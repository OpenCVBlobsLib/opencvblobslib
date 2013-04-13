#pragma once
#include "blob.h"
#include <opencv2/opencv.hpp>
using namespace cv;

class Cblob;

class Segment{
private:
	//In modo da evitare di passare 2 volte nello stesso punto, in caso di blob molto piccoli 
	bool beginVisited;
	bool endVisited;
public:
	CBlob *blobA,*blobB;
	Point begin,end;
	Segment(Point st,Point en,CBlob *bA,CBlob *bB):begin(st),end(en){blobA=bA;blobB=bB;beginVisited=false;endVisited=false;}
	void DrawSegment(Mat im,Scalar color);
	bool Contains(Point pt);
	bool isExtremum(Point pt);
	Segment();
	~Segment();
};