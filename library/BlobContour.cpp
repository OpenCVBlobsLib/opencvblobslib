#include "BlobContour.h"

using namespace cv;

CBlobContour::CBlobContour()
{
	m_startPoint.x = 0;
	m_startPoint.y = 0;
	m_area = -1;
	m_perimeter = -1;
	m_moments.m00 = -1;
	parent=NULL;
}
CBlobContour::CBlobContour(CvPoint startPoint, const Size &imageRes):m_contour(1)
{
	
	m_startPoint.x = startPoint.x;
	m_startPoint.y = startPoint.y;
	m_area = -1;
	m_perimeter = -1;
	m_moments.m00 = -1;
	parent=NULL;
	//Empirical calculations
	if(imageRes.width==-1 || imageRes.width*imageRes.height > 62500)
		m_contour[0].reserve(600);
	else{
		//I reserve a portion of the image's area
		m_contour[0].reserve(imageRes.height*imageRes.width/4);
	}
}


//! Copy constructor
CBlobContour::CBlobContour( CBlobContour *source )
{
	if (source != NULL )
	{
		*this = *source;
	}
}

CBlobContour::CBlobContour( const CBlobContour &source )
{
	m_area = source.m_area;
	m_contour = source.m_contour;
	m_contourPoints = source.m_contourPoints;
	m_moments = source.m_moments;
	m_perimeter = source.m_perimeter;
	m_startPoint = source.m_startPoint;
	parent=NULL;
}

CBlobContour::~CBlobContour()
{
}

//! Copy operator
CBlobContour& CBlobContour::operator=( const CBlobContour &source )
{
	if( this != &source )
	{		
		m_startPoint = source.m_startPoint;
		m_area = source.m_area;
		m_perimeter = source.m_perimeter;
		m_moments = source.m_moments;
		m_contour = source.m_contour;
		m_contourPoints = source.m_contourPoints;
	}
	parent = NULL;
	return *this;
}


/**
- FUNCIÓ: AddChainCode
- FUNCIONALITAT: Add chain code to contour
- PARÀMETRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACIÓ: 2008/05/06
- MODIFICACIÓ: Data. Autor. Descripció.
*/
void CBlobContour::AddChainCode(t_chainCode chaincode)
{
	m_contour[0].push_back(chaincode);
}

//! Clears chain code contour and points
void CBlobContour::Reset()
{
	m_contour.clear();
	m_contourPoints.clear();
}

/**
- FUNCIÓ: GetPerimeter
- FUNCIONALITAT: Get perimeter from chain code. Diagonals sum sqrt(2) and horizontal and vertical codes 1
- PARÀMETRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACIÓ: 2008/04/30
- MODIFICACIÓ: Data. Autor. Descripció.
- NOTA: Algorithm derived from "Methods to estimate area and perimeters of blob-like objects: A comparison", L.Yang
*/
double CBlobContour::GetPerimeter()
{
	// is calculated?
	if (m_perimeter != -1)
	{
		return m_perimeter;
	}

	if( IsEmpty() )
		return 0;

	m_perimeter = arcLength(GetContourPoints(),true);
	return m_perimeter;
}

/**
- FUNCIÓ: GetArea
- FUNCIONALITAT: Computes area from chain code
- PARÀMETRES:
	- 
- RESULTAT:
	- May give negative areas for clock wise contours
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACIÓ: 2008/04/30
- MODIFICACIÓ: Data. Autor. Descripció.
- NOTA: Algorithm derived from "Properties of contour codes", G.R. Wilson
*/
double CBlobContour::GetArea()
{
	// is calculated?
	if (m_area != -1)
	{
		return m_area;
	}

	if( IsEmpty() )
		return 0;
	m_area = fabs(contourArea(GetContourPoints(),false));
	
	return m_area;
}

//! Get contour moment (p,q up to MAX_CALCULATED_MOMENTS)
double CBlobContour::GetMoment(int p, int q)
{
	// is a valid moment?
	if ( p < 0 || q < 0 || p > MAX_MOMENTS_ORDER || q > MAX_MOMENTS_ORDER )
	{
		return -1;
	}

	if( IsEmpty() )
		return 0;

	// it is calculated?
	if( m_moments.m00 == -1)
	{
		//cvMoments( GetContourPoints(), &m_moments );
		m_moments = moments(GetContourPoints(),true);
	}
		
	return cvGetSpatialMoment( &m_moments, p, q );

}

const t_PointList CBlobContour::EMPTY_LIST = t_PointList();
//! Calculate contour points from crack codes
const t_PointList& CBlobContour::GetContourPoints()
{
	if(m_contour.size()==0)
		return EMPTY_LIST;
	if(m_contourPoints.size()!=0)
		return m_contourPoints[0];
	m_contourPoints.push_back(t_PointList());
	m_contourPoints[0].reserve(m_contour[0].size()+1);
	m_contourPoints[0].push_back(m_startPoint);
	t_chainCodeList::iterator it,en;
	it = m_contour[0].begin();
	en = m_contour[0].end();
	Point pt = m_contourPoints[0][m_contourPoints.size()-1];
	for(it;it!=en;it++){
		pt = chainCode2Point(pt,*it);
		m_contourPoints[0].push_back(pt);
	}
	return m_contourPoints[0];
}


t_contours& CBlobContour::GetContours()
{
	GetContourPoints();
	return m_contourPoints;
}


void CBlobContour::ShiftBlobContour(int x,int y)
{
	m_startPoint.x+=x;
	m_startPoint.y+=y;

	for(unsigned int j=0;j<m_contourPoints.size();j++)
		for(unsigned int i=0;i<m_contourPoints[j].size();i++)
			m_contourPoints[j][i] += Point(x,y);
}


t_chainCode points2ChainCode( CvPoint p1, CvPoint p2 )
{
	//	/* Luca Nardelli & Saverio Murgia
	//	Freeman Chain Code:	
	//		321		Values indicate the chain code used to identify next pixel location.
	//		4-0		If I join 2 blobs I can't just append the 2nd blob chain codes, since they will still start
	//		567		from the 1st blob start point
	//	*/
	Point diff = Point(p2.x-p1.x,p2.y-p1.y);
	if(diff.x == 1 && diff.y == 0)
		return 0;
	else if(diff.x == 1 && diff.y == -1)
		return 1;
	else if(diff.x == 0 && diff.y == -1)
		return 2;
	else if(diff.x == -1 && diff.y == -1)
		return 3;
	else if(diff.x == -1 && diff.y == 0)
		return 4;
	else if(diff.x == -1 && diff.y == 1)
		return 5;
	else if(diff.x == 0 && diff.y == 1)
		return 6;
	else if(diff.x == 1 && diff.y == 1)
		return 7;
	else
		return 200;
}
CvPoint chainCode2Point(CvPoint origin,t_chainCode code){
	//	/* Luca Nardelli & Saverio Murgia
	//	Freeman Chain Code:	
	//		321		Values indicate the chain code used to identify next pixel location.
	//		4-0		If I join 2 blobs I can't just append the 2nd blob chain codes, since they will still start
	//		567		from the 1st blob start point
	//	*/
	CvPoint pt = origin;
	switch(code){
	case 0:pt.x++;break;
	case 1:pt.x++;pt.y--;break;
	case 2:pt.y--;break;
	case 3:pt.x--;pt.y--;break;
	case 4:pt.x--;break;
	case 5:pt.x--;pt.y++;break;
	case 6:pt.y++;break;
	case 7:pt.x++;pt.y++;break;
	}
	return pt;
}
