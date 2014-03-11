#ifndef BLOBCONTOUR_H_INCLUDED
#define BLOBCONTOUR_H_INCLUDED


#include "list"
#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <list>


class CBlob; //Forward declaration in order to enable the "parent" field

//! Type of chain codes
typedef unsigned char t_chainCode;
//! Type of list of chain codes
typedef std::vector<t_chainCode> t_chainCodeList;
typedef std::vector<t_chainCodeList> t_chainCodeContours;	//In order to emulate CvSeq objects and to comply with opencv 2.0 interface
//! Type of list of points
typedef std::vector<cv::Point> t_PointList;
typedef std::vector<t_PointList> t_contours;


//! Max order of calculated moments
#define MAX_MOMENTS_ORDER		3

//! Blob contour class (in crack code)
class CBlobContour
{
	friend class CBlob;
	friend class myCompLabeler;
public:
	//! Constructors
	CBlobContour();
	//Size is used to empirically reserve internal vectors for contour points.
	//This can be a help for very small images, where the vector would be too large.
	CBlobContour(CvPoint startPoint, const cv::Size &imageRes = cv::Size(-1,-1));
	//! Copy constructor
	CBlobContour(CBlobContour *source );
	//CBlobContour(CBlobContour &source);
	CBlobContour(const CBlobContour &source);

	~CBlobContour();
	//! Assigment operator
	CBlobContour& operator=( const CBlobContour &source );

	//! Add point to end of contour, according to chain code.
	void AddChainCode(t_chainCode code);

	//! Return freeman chain coded contour
	t_chainCodeList& GetChainCodeList()
	{
		return m_contour[0];
	}

	bool IsEmpty()
	{
		return m_contour.size() == 0;
	}

	//! Returns first contour
	const t_PointList& GetContourPoints();
	//! Returns all contours (compatible with drawContours structure)
	t_contours& GetContours();

	void ShiftBlobContour(int x,int y);
	
	CvPoint GetStartPoint() const
	{
		return m_startPoint;
	}
protected:	

	

	//! Clears chain code contour
	void Reset();
	
	//! Computes area from contour
	double GetArea();
	//! Computes perimeter from contour
	double GetPerimeter();
	//! Get contour moment (p,q up to MAX_CALCULATED_MOMENTS)
	double GetMoment(int p, int q);

	//! Crack code list
	t_chainCodeContours m_contour;

private:
	//! Starting point of the contour
	CvPoint m_startPoint;
	//! All points from the contour
	t_contours m_contourPoints;

	//! Computed area from contour
	double m_area;
	//! Computed perimeter from contour
	double m_perimeter;
	//! Computed moments from contour
	CvMoments m_moments;
   	static const t_PointList EMPTY_LIST;

	//This value is actually used mainly in the detection part, for the labels.
	CBlob* parent;
};

t_chainCode points2ChainCode(CvPoint p1, CvPoint p2);
CvPoint chainCode2Point(CvPoint origin,t_chainCode code);

#endif	//!BLOBCONTOUR_H_INCLUDED


