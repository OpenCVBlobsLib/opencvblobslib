#include "Segment.h"


Segment::Segment(void)
{
}


Segment::~Segment(void)
{
}

void Segment::DrawSegment( Mat im,Scalar color )
{
	line(im,begin,end,color,1,8);
}

bool Segment::Contains( Point pt )
{
	return (pt.x >= begin.x && pt.x <= end.x && pt.y == begin.y);
}

bool Segment::isExtremum( Point pt )
{
	if(!beginVisited && pt == begin){
		beginVisited = true;
		return true;
	}
	else if(!endVisited && pt == end){
		endVisited=true;
		return true;
	}
	else{
		return false;
	}
}
