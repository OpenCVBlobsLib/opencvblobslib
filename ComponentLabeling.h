#if !defined(_COMPONENT_LABELING_H_INCLUDED)
#define _COMPONENT_LABELING_H_INCLUDED

#include "vector"
#include "BlobContour.h"
#include "blob.h"
#include "opencv2/opencv.hpp"
#include <pthread.h>

// vector of blob pointers typedef
typedef std::vector<CBlob*>	Blob_vector;


//My implementation of F.Chang algorithm (Luca Nardelli)
class myCompLabeler{
	friend class myCompLabelerGroup;
private:
	Mat_<int> labels;	//Mat of integers, representing blob pointers!
	int currentLabel;	//currentLabel = current Blob pointer
	int r,c,pos;
	int w,h; //Width & height of image
	uchar dir;
	static int freemanR[8],freemanC[8];
	bool singlePixBlob;

	uchar* ptrDataBinary;
	int* ptrDataLabels;

	int tempR,tempC;

	CBlob *currentBlob;
public:
	Blob_vector blobs;
	Mat binaryImage;
	Point startPoint,endPoint;
	myCompLabeler(Mat &binImage,Point start = Point(-1,-1),Point end = Point(-1,-1), const Mat &lab = Mat());
	~myCompLabeler();

	void Label();		//Do labeling in region defined by startpoint and endpoint
	void Reset(); //Resets internal buffers (label mat, etc..)
	void TracerExt();	//External contours tracer
	void TracerInt();	//Internal contours tracer
	void getNextPointCCW(); //Counter clockwise
	void getNextPointCW();  //Clockwise

	static void* thread_Labeling(void* o); //Thread function
};

class myCompLabelerGroup{
private:
	myCompLabeler** labelers;
	int numThreads;
	pthread_t *tIds;
	Mat_<int> labels;
	
public:
	myCompLabelerGroup();
	~myCompLabelerGroup();
	Mat img;
	void doLabeling(Blob_vector &blobs);
	void set(int numThreads, Mat img);
	void Reset();
};

#endif	//!_COMPONENT_LABELING_H_INCLUDED
