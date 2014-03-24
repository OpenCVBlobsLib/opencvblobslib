/************************************************************************
  			Blob.cpp
  			
- FUNCIONALITAT: Implementaci� de la classe CBlob
- AUTOR: Inspecta S.L.
MODIFICACIONS (Modificaci�, Autor, Data):

 
FUNCTIONALITY: Implementation of the CBlob class and some helper classes to perform
			   some calculations on it
AUTHOR: Inspecta S.L.
MODIFICATIONS (Modification, Author, Date):

**************************************************************************/


#include "blob.h"
#include <list>

using namespace std;
using namespace cv;

CBlob::CBlob()
{
	m_area = m_perimeter = -1;
	m_externPerimeter = m_meanGray = m_stdDevGray = -1;
	m_boundingBox.width = -1;
	m_ellipse.size.width = -1;
	m_id = -1;
	to_be_deleted=0;
	deleteRequestOwnerBlob=NULL;
	isJoined=false;
	startPassed=false;
}
CBlob::CBlob( t_labelType id, CvPoint startPoint, CvSize originalImageSize ):m_externalContour(startPoint,originalImageSize)
{
	m_externalContour.parent=this;
	m_id = id;
	m_area = m_perimeter = -1;
	m_externPerimeter = m_meanGray = m_stdDevGray = -1;
	m_boundingBox.width = -1;
	m_ellipse.size.width = -1;
	m_originalImageSize = originalImageSize;
	to_be_deleted=0;
	deleteRequestOwnerBlob=NULL;
	isJoined=false;
	startPassed=false;
}
//! Copy constructor
CBlob::CBlob( const CBlob &src )
{
	*this = src;	
}

CBlob::CBlob( const CBlob *src )
{
	if (src != NULL )
	{
		*this = *src;
	}
}

CBlob& CBlob::operator=(const CBlob &src )
{
	if( this != &src )
	{
		m_id = src.m_id;
		m_area = src.m_area;
		m_perimeter = src.m_perimeter;
		m_externPerimeter = src.m_externPerimeter;
		m_meanGray = src.m_meanGray;
		m_stdDevGray = src.m_stdDevGray;
		m_boundingBox = src.m_boundingBox;
		m_ellipse = src.m_ellipse;
		m_originalImageSize = src.m_originalImageSize;
		to_be_deleted=src.to_be_deleted;
		deleteRequestOwnerBlob=src.deleteRequestOwnerBlob;
		startPassed=false;
		// clear all current blob contours
		ClearContours();
		m_externalContour = src.m_externalContour;

		// copy all internal contours
		if( src.m_internalContours.size()!=0 )
		{
			//m_internalContours = t_contourList( src.m_internalContours.size() );
			t_CBlobContourList::const_iterator itSrc,enSrc;
			t_CBlobContourList::iterator it;

			itSrc = src.m_internalContours.begin();
			enSrc = src.m_internalContours.end();

			while (itSrc != enSrc)
			{
				m_internalContours.push_back(new CBlobContour(*itSrc));
				itSrc++;
			}
		}
	}

	isJoined = src.isJoined;
	joinedBlobs.clear();
	if(isJoined){
		list<CBlob*>::const_iterator it,en = src.joinedBlobs.end();
		for(it = src.joinedBlobs.begin();it!=en;it++){
			joinedBlobs.push_back(new CBlob(*it));
		}
	}

	return *this;
}

CBlob::~CBlob()
{
	if(isJoined){
		list<CBlob*>::iterator it,en = joinedBlobs.end();
		for(it = joinedBlobs.begin();it!=en;it++){
			delete (*it);
		}
	}

	ClearContours();
}

void CBlob::ClearContours()
{	
	t_CBlobContourList::iterator it=m_internalContours.begin(),en=m_internalContours.end();
	for(it;it!=en;it++){
		delete (*it);
	}
	m_internalContours.clear();
	m_externalContour.Reset();
}
void CBlob::AddInternalContour( const CBlobContour &newContour )
{
	m_internalContours.push_back(new CBlobContour(newContour));
}

//! Indica si el blob est� buit ( no t� cap info associada )
//! Shows if the blob has associated information
bool CBlob::IsEmpty()
{
	return GetExternalContour()->m_contour.size()==0;
}

/**
- FUNCI�: Area
- FUNCIONALITAT: Get blob area, ie. external contour area minus internal contours area
- PAR�METRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/04/30
- MODIFICACI�: Data. Autor. Descripci�.
*/
double CBlob::Area(AreaMode areaCompMode)
{
	double area=0;
	switch (areaCompMode)
	{
	case GREEN:
		{
			t_CBlobContourList::iterator itContour; 
			if(isJoined){
				list<CBlob *>::iterator it,en = joinedBlobs.end();
				for(it = joinedBlobs.begin();it!=en;it++){
					area += (*it)->Area();
				}
			}
			else{
				area = m_externalContour.GetArea();
				itContour = m_internalContours.begin();
				while (itContour != m_internalContours.end() )
				{
					if(*itContour)
						area -= (*itContour)->GetArea();
					itContour++;
				}
			}
			break;
		}
	case PIXELWISE:
		{
			Rect bbox = GetBoundingBox();
			Mat image = Mat::zeros(bbox.height,bbox.width,CV_8UC1);
			FillBlob(image,Scalar(255),-bbox.x,-bbox.y,true);
			area = countNonZero(image);
			break;
		}
	}

	return area;
}

/**
- FUNCI�: Perimeter
- FUNCIONALITAT: Get blob perimeter, ie. sum of the length of all the contours
- PAR�METRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/04/30
- MODIFICACI�: Data. Autor. Descripci�.
*/
double CBlob::Perimeter()
{
	double perimeter=0;
	t_CBlobContourList::iterator itContour; 

	if(isJoined){
		list<CBlob *>::iterator it,en = joinedBlobs.end();
		for(it = joinedBlobs.begin();it!=en;it++){
			perimeter+= (*it)->Perimeter();
		}
	}
	else{
		perimeter = m_externalContour.GetPerimeter();
		itContour = m_internalContours.begin();
		while (itContour != m_internalContours.end() )
		{
			if(*itContour)
				perimeter += (*itContour)->GetPerimeter();
			itContour++;
		}
	}

	return perimeter;

}

/**
- FUNCI�: Exterior
- FUNCIONALITAT: Return true for extern blobs
- PAR�METRES:
	- xBorder: true to consider blobs touching horizontal borders as extern
	- yBorder: true to consider blobs touching vertical borders as extern
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/05/06
- MODIFICACI�: Data. Autor. Descripci�.
*/
int	CBlob::Exterior(IplImage *mask, bool xBorder /* = true */, bool yBorder /* = true */)
{
	int result = 0;
	if (ExternPerimeter(mask, xBorder, yBorder ) > 0 )
	{
		result = 1;
	}
	
	return result;	 
}
int	CBlob::Exterior(Mat mask, bool xBorder /* = true */, bool yBorder /* = true */)
{
	IplImage temp = (IplImage) mask;
	return Exterior(&temp, xBorder, yBorder);	 
}
/**
- FUNCI�: ExternPerimeter
- FUNCIONALITAT: Get extern perimeter (perimeter touching image borders)
- PAR�METRES:
	- maskImage: if != NULL, counts maskImage black pixels as external pixels and contour points touching
				 them are counted as external contour points.
	- xBorder: true to consider blobs touching horizontal borders as extern
	- yBorder: true to consider blobs touching vertical borders as extern
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/05/05
- MODIFICACI�: Data. Autor. Descripci�.
- NOTA: If CBlobContour::GetContourPoints aproximates contours with a method different that NONE,
		this function will not give correct results
*/
double CBlob::ExternPerimeter( IplImage *maskImage, bool xBorder /* = true */, bool yBorder /* = true */)
{
	t_PointList externContour, externalPoints;
	Point actualPoint, previousPoint;
	bool find = false;
	//int i,j;
	int delta = 0;
	
	// it is calculated?
	/*if( m_externPerimeter != -1 )
	{
		return m_externPerimeter;
	}*/


	if(isJoined){
		//it an en are always different at first assignment (if isJoined is true I have at least one joined blob).
		list<CBlob *>::iterator it =joinedBlobs.begin(),en = joinedBlobs.end();
		m_externPerimeter=(*it)->ExternPerimeter(maskImage, xBorder, yBorder);
		it++;
		for(it;it!=en;it++){
			m_externPerimeter+=(*it)->ExternPerimeter(maskImage, xBorder, yBorder);
		}
		return m_externPerimeter;
	}


	// get contour pixels
	externContour = m_externalContour.GetContourPoints();
	m_externPerimeter = 0;

	// there are contour pixels?
	if( externContour.size()==0 )
	{
		return m_externPerimeter;
	}

	t_PointList::iterator it=externContour.begin(),en=externContour.end();

	previousPoint.x = -1;

	// which contour pixels touch border?
	for( it;it!=en;it++)
	{
		actualPoint = *(it);
		find = false;

		// pixel is touching border?
		if ( xBorder & ((actualPoint.x == 0) || (actualPoint.x == m_originalImageSize.width - 1 )) ||
			 yBorder & ((actualPoint.y == 0) || (actualPoint.y == m_originalImageSize.height - 1 )))
		{
			find = true;
		}
		else
		{
			if( maskImage != NULL )
			{
				// verify if some of 8-connected neighbours is black in mask
				char *pMask;
				
				pMask = (maskImage->imageData + actualPoint.x - 1 + (actualPoint.y - 1) * maskImage->widthStep);
				
				for ( int i = 0; i < 3; i++, pMask++ )
				{
					if(*pMask == 0 && !find ) 
					{
						find = true;
						break;
					}						
				}
				
				if(!find)
				{
					pMask = (maskImage->imageData + actualPoint.x - 1 + (actualPoint.y ) * maskImage->widthStep);
				
					for ( int i = 0; i < 3; i++, pMask++ )
					{
						if(*pMask == 0 && !find ) 
						{
							find = true;
							break;
						}
					}
				}
			
				if(!find)
				{
					pMask = (maskImage->imageData + actualPoint.x - 1 + (actualPoint.y + 1) * maskImage->widthStep);

					for ( int i = 0; i < 3; i++, pMask++ )
					{
						if(*pMask == 0 && !find ) 
						{
							find = true;
							break;
						}
					}
				}
			}
		}

		if( find )
		{
			if( previousPoint.x > 0 )
				delta = abs(previousPoint.x - actualPoint.x) + abs(previousPoint.y - actualPoint.y);

			// calculate separately each external contour segment 
			if( delta > 2 )
			{
				m_externPerimeter += arcLength( externalPoints,false);
				
				externalPoints.clear();
				delta = 0;
				previousPoint.x = -1;
			}
			externalPoints.push_back(actualPoint);
			previousPoint = actualPoint;
		}
		
	}

	if(externalPoints.size()!=0)
		m_externPerimeter += arcLength( externalPoints,false);
	// divide by two because external points have one side inside the blob and the other outside
	// Perimeter of external points counts both sides, so it must be divided
	m_externPerimeter /= 2.0;
	return m_externPerimeter;
}
double CBlob::ExternPerimeter( Mat maskImage, bool xBorder /* = true */, bool yBorder /* = true */){
	if(!maskImage.data){
		return ExternPerimeter( NULL, xBorder /* = true */, yBorder /* = true */);
	}
	else{
		IplImage temp = (IplImage) maskImage;
		return ExternPerimeter( &temp, xBorder /* = true */, yBorder /* = true */);
	}
}
//! Compute blob's moment (p,q up to MAX_CALCULATED_MOMENTS)
double CBlob::Moment(int p, int q)
{
	double moment=0;
	t_CBlobContourList::iterator itContour; 

	if(isJoined){
		list<CBlob *>::iterator it,en = joinedBlobs.end();
		for(it = joinedBlobs.begin();it!=en;it++){
			moment += (*it)->Moment(p,q);
		}
	}
	else{
		moment = m_externalContour.GetMoment(p,q);
		itContour = m_internalContours.begin();
		while (itContour != m_internalContours.end() )
		{
			moment -= (*itContour)->GetMoment(p,q);
			itContour++;
		}
	}
	return moment;
}

/**
- FUNCI�: Mean
- FUNCIONALITAT: Get blob mean color in input image
- PAR�METRES:
	- image: image from gray color are extracted
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/05/06
- MODIFICACI�: Data. Autor. Descripci�.
*/
double CBlob::Mean( IplImage *image )
{
	// Create a mask with same size as blob bounding box
	IplImage *mask;
	CvScalar mean, std;
	CvPoint offset;

	GetBoundingBox();
	
	if (m_boundingBox.height == 0 ||m_boundingBox.width == 0 || !CV_IS_IMAGE( image ))
	{
		m_meanGray = 0;
		return m_meanGray;
	}

	// apply ROI and mask to input image to compute mean gray and standard deviation
	mask = cvCreateImage( cvSize(m_boundingBox.width, m_boundingBox.height), IPL_DEPTH_8U, 1);
	cvSetZero(mask);

	offset.x = -m_boundingBox.x;
	offset.y = -m_boundingBox.y;

  Mat mask_mat(mask);
      
	//If joined
	if(isJoined){
		list<CBlob *>::iterator it,en = joinedBlobs.end();
		for(it = joinedBlobs.begin();it!=en;it++){
// 			cvDrawContours( mask, (*it)->m_externalContour.GetContourPoints(), CV_RGB(255,255,255), CV_RGB(255,255,255),0, CV_FILLED, 8,
// 				offset );
// 			vector<t_PointList> conts;
// 			conts.push_back((*it)->m_externalContour.GetContourPoints());
			drawContours(mask_mat,(*it)->m_externalContour.GetContours(),-1,CV_RGB(255,255,255),CV_FILLED,8,noArray(),2147483647,offset);
			t_CBlobContourList::iterator itint = (*it)->m_internalContours.begin();
			while(itint != (*it)->m_internalContours.end() )
			{
// 				cvDrawContours( mask, (*itint).GetContourPoints(), CV_RGB(0,0,0), CV_RGB(0,0,0),0, CV_FILLED, 8,
// 					offset );
				drawContours(mask_mat,(*itint)->GetContours(),-1,CV_RGB(0,0,0),CV_FILLED,8,noArray(),2147483647,offset);
				itint++;
			}
		}
	}
	else{
		// draw contours on mask
// 		cvDrawContours( mask, m_externalContour.GetContourPoints(), CV_RGB(255,255,255), CV_RGB(255,255,255),0, CV_FILLED, 8,
// 						offset );
// 		vector<t_PointList> conts;
// 		conts.push_back(m_externalContour.GetContourPoints());
		drawContours(mask_mat,m_externalContour.GetContours(),-1,CV_RGB(255,255,255),CV_FILLED,8,noArray(),2147483647,offset);
		// draw internal contours
		t_CBlobContourList::iterator it = m_internalContours.begin();
		while(it != m_internalContours.end() )
		{
// 			cvDrawContours( mask, (*it).GetContourPoints(), CV_RGB(0,0,0), CV_RGB(0,0,0),0, CV_FILLED, 8,
// 						offset );
			drawContours(mask_mat,(*it)->GetContours(),-1,CV_RGB(0,0,0),CV_FILLED,8,noArray(),2147483647,offset);
			it++;
		}
	}

	cvSetImageROI( image, m_boundingBox );
	cvAvgSdv( image, &mean, &std, mask );
	
	m_meanGray = mean.val[0];
	m_stdDevGray = std.val[0];

	cvReleaseImage( &mask );
	cvResetImageROI( image );

	return m_meanGray;
}

double CBlob::Mean(Mat image ){
	IplImage temp = (IplImage) image;
	return Mean(&temp);
}
double CBlob::StdDev( IplImage *image )
{
	// call mean calculation (where also standard deviation is calculated)
	Mean( image );

	return m_stdDevGray;
}
double CBlob::StdDev(Mat image){
	IplImage temp = (IplImage) image;
	return StdDev(&temp);
}

//void CBlob::MeanStdDev( Mat image, double *mean, double *stddev )
//{
//	IplImage temp = (IplImage) image;
//	Mean(&temp);
//	*mean = m_meanGray;
//	*stddev = m_stdDevGray;
//	return;
//}

void CBlob::MeanStdDev( Mat image, Scalar &mean, Scalar &stddev )
{
	GetBoundingBox();
	// Create a mask with same size as blob bounding box and set it to 0
	Mat mask_mat = Mat(m_boundingBox.height,m_boundingBox.width,CV_8UC1,Scalar(0));
	Point offset(-m_boundingBox.x,-m_boundingBox.y);

	//If joined
	if(isJoined){
		list<CBlob *>::iterator it,en = joinedBlobs.end();
		for(it = joinedBlobs.begin();it!=en;it++){
			drawContours(mask_mat,(*it)->m_externalContour.GetContours(),-1,CV_RGB(255,255,255),CV_FILLED,8,noArray(),2147483647,offset);
			t_CBlobContourList::iterator itint = (*it)->m_internalContours.begin();
			while(itint != (*it)->m_internalContours.end() )
			{
				drawContours(mask_mat,(*itint)->GetContours(),-1,CV_RGB(0,0,0),CV_FILLED,8,noArray(),2147483647,offset);
				itint++;
			}
		}
	}
	else{
		// draw contours on mask
		drawContours(mask_mat,m_externalContour.GetContours(),-1,CV_RGB(255,255,255),CV_FILLED,8,noArray(),2147483647,offset);
		// draw internal contours
		t_CBlobContourList::iterator it = m_internalContours.begin();
		while(it != m_internalContours.end() )
		{
			drawContours(mask_mat,(*it)->GetContours(),-1,CV_RGB(0,0,0),CV_FILLED,8,noArray(),2147483647,offset);
			it++;
		}
	}

	cv::meanStdDev(image(m_boundingBox),mean,stddev,mask_mat);
}

/**
- FUNCI�: GetBoundingBox
- FUNCIONALITAT: Get bounding box (without rotation) of a blob
- PAR�METRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/05/06
- MODIFICACI�: Data. Autor. Descripci�.
*/
CvRect CBlob::GetBoundingBox()
{
	// it is calculated?
	if( m_boundingBox.width != -1 )
	{
		return m_boundingBox;
	}

	if(isJoined){
		CvRect bigRect;
		bigRect.x = 1000000;
		bigRect.y = 1000000;
		bigRect.height = 0;
		bigRect.width = 0;
		list<CBlob *>::iterator it,en = joinedBlobs.end();
		int maxX=0,maxY=0;
		for(it = joinedBlobs.begin();it!=en;it++){
			CvRect temp = (*it)->GetBoundingBox();
			if(bigRect.x > temp.x){
				bigRect.x = temp.x;
			}
			if(bigRect.y > temp.y){
				bigRect.y = temp.y;
			}
			//-1 in order to obtain the correct measure (I'm looking for the points)
			if(maxX < temp.x+temp.width){
				maxX = temp.x+temp.width-1;
			}
			if(maxY < temp.y + temp.height){
				maxY = temp.y + temp.height-1;
			}
		}
		//+1 in order to obtain the correct measure
		bigRect.width=maxX - bigRect.x+1;
		bigRect.height=maxY - bigRect.y+1;
		m_boundingBox=bigRect;
		return m_boundingBox;
	}

	t_PointList externContour;
	
	// get contour pixels
	externContour = m_externalContour.GetContourPoints();

	// it is an empty blob?
	m_boundingBox.x = 1000000;
	m_boundingBox.y = 1000000;
	m_boundingBox.width = 0;
	m_boundingBox.height = 0;


	t_PointList::iterator it=externContour.begin(),en=externContour.end();
	for( it;it!=en;it++)
	{
		Point &actualPoint = *it;

		m_boundingBox.x = MIN( actualPoint.x, m_boundingBox.x );
		m_boundingBox.y = MIN( actualPoint.y, m_boundingBox.y );
		
		m_boundingBox.width = MAX( actualPoint.x, m_boundingBox.width );
		m_boundingBox.height = MAX( actualPoint.y, m_boundingBox.height );
	}

	//+1 in order to take into account single pixel blobs.
	//In this case, a single pixel has a bounding box like Rect(x,y,1,1), which is ok with opencv functions
	m_boundingBox.width = m_boundingBox.width - m_boundingBox.x+1;
	m_boundingBox.height = m_boundingBox.height - m_boundingBox.y+1;

	return m_boundingBox;
}

/**
- FUNCI�: GetEllipse
- FUNCIONALITAT: Calculates bounding ellipse of external contour points
- PAR�METRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/05/06
- MODIFICACI�: Data. Autor. Descripci�.
- NOTA: Calculation is made using second order moment aproximation
*/
CvBox2D CBlob::GetEllipse()
{
	// it is calculated?
	if( m_ellipse.size.width != -1 )
		return m_ellipse;
	
	double u00,u11,u01,u10,u20,u02, delta, num, den, temp;

	// central moments calculation
	u00 = Moment(0,0);

	// empty blob?
	if ( u00 <= 0 )
	{
		m_ellipse.size.width = 0;
		m_ellipse.size.height = 0;
		m_ellipse.center.x = 0;
		m_ellipse.center.y = 0;
		m_ellipse.angle = 0;
		return m_ellipse;
	}
	u10 = Moment(1,0) / u00;
	u01 = Moment(0,1) / u00;

	u11 = -(Moment(1,1) - Moment(1,0) * Moment(0,1) / u00 ) / u00;
	u20 = (Moment(2,0) - Moment(1,0) * Moment(1,0) / u00 ) / u00;
	u02 = (Moment(0,2) - Moment(0,1) * Moment(0,1) / u00 ) / u00;


	// elipse calculation
	delta = sqrt( 4*u11*u11 + (u20-u02)*(u20-u02) );
	m_ellipse.center.x = (float)u10;
	m_ellipse.center.y = (float)u01;
	
	temp = u20 + u02 + delta;
	if( temp > 0 )
	{
		m_ellipse.size.width = (float)sqrt( 2*(u20 + u02 + delta ));
	}	
	else
	{
		m_ellipse.size.width = 0;
		return m_ellipse;
	}

	temp = u20 + u02 - delta;
	if( temp > 0 )
	{
		m_ellipse.size.height = (float)sqrt( 2*(u20 + u02 - delta ) );
	}
	else
	{
		m_ellipse.size.height = 0;
		return m_ellipse;
	}

	// elipse orientation
	if (u20 > u02)
	{
		num = u02 - u20 + sqrt((u02 - u20)*(u02 - u20) + 4*u11*u11);
		den = 2*u11;
	}
    else
    {
		num = 2*u11;
		den = u20 - u02 + sqrt((u20 - u02)*(u20 - u02) + 4*u11*u11);
    }
	if( num != 0 && den  != 00 )
	{
		m_ellipse.angle = (float)(180.0 + (180.0 / CV_PI) * atan( num / den ));
	}
	else
	{
		m_ellipse.angle = 0;
	}
        
	return m_ellipse;

}

/**
- FUNCTION: FillBlob
- FUNCTIONALITY: 
	- Fills the blob with a specified colour
- PARAMETERS:
	- image: where to paint
	- color: colour to paint the blob
	- offset: point offset for drawing
	- intContours: do not paint the internal holes (leave them transparent)
	- srcImage: image from where to copy the internal holes contents
- RESULT:
	- modified input image
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION:
	- sep/2013. Luca Nardelli. Added functionality to consider internal contours when filling the blob.
*/
void CBlob::FillBlob( IplImage *image, CvScalar color, int offsetX , int offsetY, bool intContours, const IplImage *srcImage) 					  
{
	if(srcImage==NULL)
		FillBlob(Mat(image),color,offsetX,offsetY,intContours,Mat());
	else
		FillBlob(Mat(image),color,offsetX,offsetY,intContours,Mat(srcImage));
}
void CBlob::FillBlob( Mat image, CvScalar color, int offsetX, int offsetY, bool intContours, const Mat srcImage){
	CV_FUNCNAME("CBlob::FillBlob");
	__CV_BEGIN__;
	if(srcImage.data && intContours)
		CV_ASSERT(image.size()==srcImage.size() && image.type() == srcImage.type());
  {
    Rect bbox = GetBoundingBox();
    Point drawOffset(offsetX,offsetY);
    Size imSz = image.size();
    if(bbox.x+offsetX+bbox.width >= imSz.width){
      bbox.width = imSz.width - bbox.x-offsetX;
    }
    else if(bbox.x+offsetX < 0){
      bbox.x = -offsetX;
      bbox.width= bbox.width +offsetX;
    }
    if(bbox.y+offsetY+bbox.height >= imSz.height){
      bbox.height = imSz.height - bbox.y-offsetY;
    }
    else if(bbox.y+offsetY < 0){
      bbox.y = -offsetY;
      bbox.height= bbox.height +offsetY;
    }
    if(bbox.width <0 || bbox.height <0){
      return;
    }
    if(bbox.width==0)
      bbox.width++;
    if(bbox.height==0)
      bbox.height++;
    if(isJoined){
      list<CBlob *>::iterator itBlob=joinedBlobs.begin(),enBlob = joinedBlobs.end();
      for(itBlob = joinedBlobs.begin();itBlob!=enBlob;itBlob++){
        (*itBlob)->FillBlob(image,color,offsetX,offsetY,intContours,srcImage);
      }
      // 		if(intContours){
      // 			Point offset(-bbox.x,-bbox.y);
      // 			Size sz(bbox.width,bbox.height);
      // 			Mat temp(sz,image.type());
      // 			Mat mask(sz,CV_8UC1);
      // 			mask.setTo(0);
      // 			for(itBlob = joinedBlobs.begin();itBlob!=enBlob;itBlob++){
      // 				CBlob *curBlob = *itBlob;
      // 				t_CBlobContourList::iterator it = curBlob->m_internalContours.begin(),en = curBlob->m_internalContours.end();
      // 				for(it;it!=en;it++){
      // 					drawContours(mask,(*it)->GetContours(),-1,255,CV_FILLED,8,noArray(),2147483647,offset);
      // 				}
      // 			}
      // 			srcImage(bbox).copyTo(temp,mask);
      // 			for(itBlob = joinedBlobs.begin();itBlob!=enBlob;itBlob++){
      // 				drawContours(image,(*itBlob)->m_externalContour.GetContours(),-1,color,CV_FILLED,8,noArray(),2147483647,drawOffset);
      // 			}
      // 			temp.copyTo(image(bbox+drawOffset),mask);
      // 			for(itBlob = joinedBlobs.begin();itBlob!=enBlob;itBlob++){
      // 				CBlob *curBlob = *itBlob;
      // 				t_CBlobContourList::const_iterator it = curBlob->m_internalContours.begin(),en = curBlob->m_internalContours.end();
      // 				for(it;it!=en;it++){
      // 					drawContours(image,(*it)->GetContours(),-1,color,1,8,noArray(),2147483647,drawOffset);
      // 				}
      // 			}
      // 		}
      // 		else{
      // 			for(itBlob = joinedBlobs.begin();itBlob!=enBlob;itBlob++){
      // 				drawContours(image,(*itBlob)->m_externalContour.GetContours(),-1,color,CV_FILLED,8,noArray(),2147483647,drawOffset);
      // 			}
      // 		}
    }
    else{
      if(intContours){
        Point offset(-bbox.x,-bbox.y);
        t_CBlobContourList::iterator it = m_internalContours.begin(),en = m_internalContours.end();
        Mat temp(bbox.height,bbox.width,image.type());
        drawContours(image,m_externalContour.GetContours(),-1,color,CV_FILLED,8,noArray(),2147483647,drawOffset);
        if(srcImage.data){
          Mat mask(bbox.height,bbox.width,CV_8UC1);
          mask.setTo(0);
          for(it;it!=en;it++){
            drawContours(mask,(*it)->GetContours(),-1,255,CV_FILLED,8,noArray(),2147483647,offset);
          }
          srcImage(bbox).copyTo(temp,mask);
          Mat image_roi = image(bbox+drawOffset);
          temp.copyTo(image_roi,mask);
        }
        else{
          for(it;it!=en;it++){
            drawContours(image,(*it)->GetContours(),-1,CV_RGB(0,0,0),CV_FILLED,8,noArray(),2147483647,drawOffset);
          }
        }
			
        for(it=m_internalContours.begin();it!=en;it++){
          drawContours(image,(*it)->GetContours(),-1,color,1,8,noArray(),2147483647,drawOffset);
        }
      }
      else
        drawContours(image,m_externalContour.GetContours(),-1,color,CV_FILLED,8,noArray(),2147483647,drawOffset);
    }
  }
	__CV_END__;
}

/**
- FUNCTION: GetConvexHull
- FUNCTIONALITY: Calculates the convex hull polygon of the blob
- PARAMETERS:
	- dst: where to store the result
- RESULT:
	- true if no error ocurred
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void CBlob::GetConvexHull( t_contours& hull )
{
	hull.clear();
	t_PointList extCont;
	hull.push_back(t_PointList());
	if(isJoined){
		int numPts = 0;
		t_blobList::iterator it,en=joinedBlobs.end();
		for(it=joinedBlobs.begin();it!=en;it++){
			numPts+= (*it)->GetExternalContour()->GetContourPoints().size();
		}
		hull[0].reserve(numPts);
		extCont.reserve(numPts);
		for(it=joinedBlobs.begin();it!=en;it++){
			const t_PointList& pts = (*it)->GetExternalContour()->GetContourPoints();
			extCont.insert(extCont.end(),pts.begin(),pts.end());
		}
	}
	else{
		extCont = m_externalContour.GetContourPoints();	
	}
	cv::convexHull(extCont,hull[0],true,true);
}

/**
- FUNCTION: JoinBlob
- FUNCTIONALITY: Joins the 2 blobs, creating another blob which contains the 2 joined ones
- PARAMETERS:
	- blob: blob to join with the calling one
- RESULT:
	- Joined blob
- RESTRICTIONS: Only external contours are added
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: 
	08-2013, Luca Nardelli & Saverio Murgia, Created a working version of the join blob function
*/
void CBlob::JoinBlob( CBlob *blob)
{
	/* Luca Nardelli & Saverio Murgia */
	//Check on m_storage in order to not add empty blobs.
	if(!isJoined && !IsEmpty()){
		this->joinedBlobs.push_back(new CBlob(this));
	}
	if(blob->isJoined){
		list<CBlob *>::iterator it,en = blob->joinedBlobs.end();
		for(it = blob->joinedBlobs.begin();it!=en;it++){
			this->joinedBlobs.push_back(new CBlob(*it));
		}
	}
	else{
		this->joinedBlobs.push_back(new CBlob(blob));
	}

	this->isJoined=true;
	this->m_boundingBox.width=-1;
	this->m_externPerimeter=-1;
	this->m_meanGray=-1;

}


void CBlob::requestDeletion( CBlob *blob )
{
	//Se il blob � gi� stato segnalato per la cancellazione, allora segnalo anche i blob che lo hanno richiesto per essere cancellati
	while(blob->deleteRequestOwnerBlob!=NULL){
		CBlob *temp = blob;
		blob->deleteRequestOwnerBlob->to_be_deleted=1;
		blob = blob->deleteRequestOwnerBlob;
		temp->deleteRequestOwnerBlob=this;
	}
	blob->to_be_deleted=1;
	blob->deleteRequestOwnerBlob=this;
}

int CBlob::getNumJoinedBlobs()
{
	return joinedBlobs.size();
}

void CBlob::ShiftBlob( int x,int y )
{
	m_externalContour.ShiftBlobContour(x,y);
	t_CBlobContourList::iterator it,en=m_internalContours.end();
	for(it=m_internalContours.begin();it!=en;it++){
		(*it)->ShiftBlobContour(x,y);
	}
	m_boundingBox.x += x;
	m_boundingBox.y += y;
}

Point CBlob::getCenter()
{
	return Point((int)(GetBoundingBox().x+GetBoundingBox().width*0.5),(int)(GetBoundingBox().y+GetBoundingBox().height*0.5));
}

int CBlob::overlappingPixels(CBlob *blob )
{
	Rect r1 = GetBoundingBox(),r2 = blob->GetBoundingBox();
	Rect interRect = r1 & r2;
	if(interRect.width == 0 || interRect.height==0)
		return 0;
	Rect minContainingRect = r1 | r2; //Minimum containing rectangle
	Mat m1 = Mat::zeros(minContainingRect.height,minContainingRect.width,CV_8UC1);
	Mat m2 = Mat::zeros(minContainingRect.height,minContainingRect.width,CV_8UC1);
	FillBlob(m1,Scalar(255),-minContainingRect.x,-minContainingRect.y,true);
	blob->FillBlob(m2,Scalar(255),-minContainingRect.x,-minContainingRect.y,true);
	return countNonZero(m1&m2);
}

double CBlob::density( AreaMode areaCalculationMode )
{
	double density = 0;
	t_CBlobContourList::iterator itContour; 
	switch (areaCalculationMode){
	case GREEN:
		{
			double blobArea = Area();
			t_contours cHull,cHullPts;
			GetConvexHull(cHull);
			//approxPolyDP(cHull, cHullPts, 0.001, true);
			double cHullArea = fabs(contourArea(cHull[0],false));
			density = blobArea/cHullArea;
			break;
		}
	case PIXELWISE:
		{
			t_contours cHull;
			GetConvexHull(cHull);
			Rect bbox = GetBoundingBox();
			Mat blMat = Mat::zeros(bbox.height,bbox.width,CV_8UC1);
			Mat cHullMat = Mat::zeros(bbox.height,bbox.width,CV_8UC1);
			FillBlob(blMat,Scalar(255),-bbox.x,-bbox.y,true);
			drawContours(cHullMat,cHull,-1,Scalar(255),-1,8,noArray(),2147483647,Point(-bbox.x,-bbox.y));
			int totArea = countNonZero(cHullMat);
			int actArea = countNonZero(blMat);
			density = (double)actArea/totArea;
			break;
		}
	}
	
	return density;
}


