#pragma once
#include <opencv2/opencv.hpp>
using namespace cv;
class Segment{
public:
	Point begin,end;
	Segment(Point st,Point en):begin(st),end(en){}
	Segment();
	~Segment();
};