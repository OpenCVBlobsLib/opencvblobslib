/************************************************************************
  			Blob.h
  			
FUNCIONALITAT: Definici� de la classe CBlob
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificaci�, Autor, Data):

FUNCTIONALITY: Definition of the CBlob class and some helper classes to perform
			   some calculations on it
AUTHOR: Inspecta S.L.
MODIFICATIONS (Modification, Author, Date):

**************************************************************************/

//! Disable warnings referred to 255 character truncation for the std:map
#pragma warning( disable : 4786 ) 

#ifndef CBLOB_INSPECTA_INCLUDED
#define CBLOB_INSPECTA_INCLUDED
class CBlob;
#include "opencv/cxcore.h"
#include "opencv2/opencv.hpp"
#include "BlobLibraryConfiguration.h"
#include "BlobContour.h"
#include <deque>
#include <list>


#ifdef BLOB_OBJECT_FACTORY
	//! Object factory pattern implementation
	#include "..\inspecta\DesignPatterns\ObjectFactory.h"
#endif

//! Type of labelled images
typedef unsigned int t_labelType;
typedef std::list<CBlob*> t_blobList;
typedef std::list<CBlobContour*> t_CBlobContourList;

enum AreaMode {GREEN, PIXELWISE};

//! Blob class
class CBlob
{
	friend class myCompLabeler;
public:
	CBlob();
	CBlob( t_labelType id, CvPoint startPoint, CvSize originalImageSize );
	~CBlob();

	//! Copy constructor
	CBlob( const CBlob &src );
	CBlob( const CBlob *src );

	//! Operador d'assignaci�
	//! Assigment operator
	CBlob& operator=(const CBlob &src );

	//! Adds a new internal contour to the blob
	void AddInternalContour( const CBlobContour &newContour );
	
	//! Retrieves contour in Freeman's chain code
	CBlobContour *GetExternalContour()
	{
		return &m_externalContour;
	}

	t_CBlobContourList& GetInternalContours(){
		return m_internalContours;
	}

	//! Bool to permit deletion with filter function
	double to_be_deleted;

	//! Get label ID
	t_labelType GetID()
	{
		return m_id;
	}
	void SetID(t_labelType newID){
		m_id=newID;
	}
	//! > 0 for extern blobs, 0 if not
	int	  Exterior( IplImage *mask, bool xBorder = true, bool yBorder = true );
	//! opencv2 Interface
	int	  Exterior( cv::Mat mask, bool xBorder = true, bool yBorder = true );
	//Computes the area of the blob.
	// areaCompMode defines which way to compute the areas:
	// - Using green's formula (not exact result, probably faster)
	// - Counting the pixels (probably slower)
	double Area(AreaMode areaCompMode = GREEN);
	//! Compute blob's perimeter
	double Perimeter();
	//! Compute blob's moment (p,q up to MAX_CALCULATED_MOMENTS)
	double Moment(int p, int q);

	//! Compute extern perimeter 
	double ExternPerimeter( IplImage *mask, bool xBorder  = true, bool yBorder = true );
	//! opencv2 interface
	double ExternPerimeter( cv::Mat mask, bool xBorder  = true, bool yBorder = true );
	
	//! Get mean grey color
	//(Warning: use MeanStdDev for simultaneous computation of mean and std. dev, and for RGB images).
	double Mean( IplImage *image );
	//! opencv2 interface
	//(Warning: use MeanStdDev for simultaneous computation of mean and std. dev, and for RGB images).
	double Mean( cv::Mat image );
	//! Get standard deviation grey color
	//(Warning: use MeanStdDev for simultaneous computation of mean and std. dev, and for RGB images).
	double StdDev( IplImage *image );
	//! opencv2 interface
	//(Warning: use MeanStdDev for simultaneous computation of mean and std. dev, and for RGB images).
	double StdDev( cv::Mat image );

	//Computes mean and standard deviation of image, which can be in any opencv format
	//Since mean and standard deviation are computed with the same function call, this results quicker than
	//calling separately mean and standard deviation.
	void MeanStdDev(cv::Mat image, cv::Scalar &mean, cv::Scalar &stddev);

	//void MeanStdDev(Mat image, double *mean, double *stddev);

	//! Indica si el blob est� buit ( no t� cap info associada )
	//! Shows if the blob has associated information
	bool IsEmpty();

	//! Retorna el poligon convex del blob
	//! Calculates the convex hull of the blob
	void GetConvexHull(t_contours& hull);

	//! Pinta l'interior d'un blob d'un color determinat
	//!  Paints the blob in an image
	//!	intContours - determines wheter to draw the holes of the blob (true) or not (false)
	//!	srcImage - image from where to copy the holes contents. If unassigned and intContours is true, the internal pixels will be set to black.
	void FillBlob( IplImage *image, CvScalar color, int offsetX = 0, int offsetY = 0, bool intContours = false, const IplImage *srcImage = NULL );
	void FillBlob( cv::Mat image, CvScalar color, int offsetX = 0, int offsetY = 0, bool intContours = false, const cv::Mat srcImage = cv::Mat() );
	
	//! Joins a blob to current one
	//! NOTE: All the data is copied, a new blob is created and joined to the caller one.
	void JoinBlob( CBlob *blob);

	//! Get bounding box
	CvRect GetBoundingBox();
	//! Get bounding ellipse
	CvBox2D GetEllipse();

	//! Minimun X	
	double MinX()
	{
		return GetBoundingBox().x;
	}
	//! Minimun Y
	double MinY()
	{
		return GetBoundingBox().y;
	}
	//! Maximun X
	double MaxX()
	{
		return GetBoundingBox().x + GetBoundingBox().width;
	}
	//! Maximun Y
	double MaxY()
	{
		return GetBoundingBox().y + GetBoundingBox().height;
	}

	//Shifts the blob by (x,y) 
	void ShiftBlob(int x,int y);

	//Returns the number of overlapping pixels between the caller blob and blob.
	//A preliminary check is performed with respect to the bounding boxes in order to avoid unnecessary computations
	int overlappingPixels(CBlob *blob);

	//Computes the density of the blob, i.e. the ratio (blob Area) / (ConvexHullArea)
	// areaCalculationMode defines which way to compute the areas:
	// - Using green's formula (not exact result, probably faster)
	// - Counting the pixels
	double density(AreaMode areaCalculationMode);

	//Returns blob center in pixels (integers).
	cv::Point getCenter();
	/*
	Border: 0 = top, 1 = right, 2 = bottom, 3 = left
	*/
	/*t_contours getPointsTouchingBorder(int border);*/
	
	int getNumJoinedBlobs(); // For joined blobs, return the number of sub-blobs.

private:
	//Just for multithread joining routine;
	bool startPassed;

	bool isJoined;
	t_blobList joinedBlobs;

	CBlob* deleteRequestOwnerBlob;
	void requestDeletion(CBlob *blob);


	//! Deallocates all contours
	void ClearContours();
	//////////////////////////////////////////////////////////////////////////
	// Blob contours
	//////////////////////////////////////////////////////////////////////////

	//! External contour of the blob (crack codes)
	CBlobContour m_externalContour;
	//! Internal contours (crack codes)
	t_CBlobContourList m_internalContours;

	//////////////////////////////////////////////////////////////////////////
	// Blob features
	//////////////////////////////////////////////////////////////////////////
	
	//! Label number
	t_labelType m_id;
	//! Area
	double m_area;
	//! Perimeter
	double m_perimeter;
	//! Extern perimeter from blob
	double m_externPerimeter;
	//! Mean gray color
	double m_meanGray;
	//! Standard deviation from gray color blob distribution
	double m_stdDevGray;
	//! Bounding box
	CvRect m_boundingBox;
	//! Bounding ellipse
	CvBox2D m_ellipse;

	//! Sizes from image where blob is extracted
	CvSize m_originalImageSize;
	public: CvSize OriginalImageSize() const { return m_originalImageSize; }
	public: void OriginalImageSize(int width, int height) { m_originalImageSize.width = width; m_originalImageSize.height = height; }


	friend class CBlobResult;
};




#endif //CBLOB_INSPECTA_INCLUDED
