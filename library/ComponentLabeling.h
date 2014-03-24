#if !defined(_COMPONENT_LABELING_H_INCLUDED)
#define _COMPONENT_LABELING_H_INCLUDED

//#define DEBUG_COMPONENT_LABELLING

#include "vector"
#include "BlobContour.h"
#include "blob.h"
#include "opencv2/opencv.hpp"
#include <pthread.h>

class myCompLabelerGroup; //forward decl. for "parent" field of myCompLabeler.

// vector of blob pointers typedef
typedef std::vector<CBlob*>	Blob_vector;


//My implementation of F.Chang algorithm (Luca Nardelli)
class myCompLabeler{
	friend class myCompLabelerGroup;
private:
	myCompLabelerGroup *parent; //myCompLabelerGroup parent, in order to get access to mutexes.
	CBlobContour** labels;
	int currentLabel;	//currentLabel
	int r,c,pos;
	int w,h; //Width& Height of image
	uchar dir;
	static int freemanR[8],freemanC[8];
	bool singlePixBlob;

	uchar* ptrDataBinary;
	CBlobContour** ptrDataLabels;

	int tempR,tempC;

	CBlob *currentBlob;
	CBlobContour *currentContour;
public:
	Blob_vector blobs;
	cv::Mat binaryImage;
	cv::Point startPoint,endPoint;
	//Double pointer so to pass the array of blob pointers
	myCompLabeler(cv::Mat &binImage,CBlobContour** lab,cv::Point start = cv::Point(-1,-1),cv::Point end = cv::Point(-1,-1));
	~myCompLabeler();

	void Label();		//Do labeling in region defined by startpoint and endpoint
	void Reset(); //Resets internal buffers
	void TracerExt();	//External contours tracer
	void TracerInt(int startDir = 5);	//Internal contours tracer
	void getNextPointCCW(); //Counter clockwise
	void getNextPointCW();  //Clockwise

	static void* thread_Labeling(void* o); //Thread function
};

class myCompLabelerGroup{
private:
	myCompLabeler** labelers;
	int numThreads;
	pthread_t *tIds;
	pthread_mutex_t mutexBlob;
	//Mat_<int> labels;
	CBlobContour** labels;

	void acquireMutex();
	void releaseMutex();

public:
	myCompLabelerGroup();
	~myCompLabelerGroup();
	cv::Mat img;
	void doLabeling(Blob_vector &blobs);
	void set(int numThreads, cv::Mat img);
	void Reset();

friend class myCompLabeler;
};

#endif	//!_COMPONENT_LABELING_H_INCLUDED
