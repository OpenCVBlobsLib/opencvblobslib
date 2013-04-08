#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "blob.h"
#include "BlobResult.h"

using namespace std;
int main(){
	RNG random;
	Mat color_img = imread("opencv.png");
	namedWindow("Color Image",CV_WINDOW_NORMAL);
	namedWindow("Gray Image",CV_WINDOW_NORMAL);
	namedWindow("Binary Image",CV_WINDOW_NORMAL);
	namedWindow("Blobs Image",CV_WINDOW_NORMAL);
	imshow("Color Image",color_img);
	Mat binary_img(color_img.size(),CV_8UC1);
	cvtColor(color_img,binary_img,CV_BGR2GRAY);
	imshow("Gray Image",binary_img);
	threshold(binary_img,binary_img,250,255,CV_THRESH_BINARY_INV);
	imshow("Binary Image",binary_img);
	CBlobResult blobs(binary_img,Mat(),0);
	CBlob *curblob;
	cout<<"found: "<<blobs.GetNumBlobs();
	stringstream s;
	blobs.GetBlob(0)->JoinBlob(blobs.GetBlob(4));
	blobs.GetBlob(0)->JoinBlob(blobs.GetBlob(5));
	for(int i=0;i<blobs.GetNumBlobs();i++){
		curblob=blobs.GetBlob(i);
		curblob->FillBlob(color_img,CV_RGB(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255)));
		s<<i;
		putText(color_img,s.str(),Point(curblob->GetBoundingBox().x+curblob->GetBoundingBox().width*0.5,curblob->GetBoundingBox().y+curblob->GetBoundingBox().height*0.5),1.6,5,CV_RGB(0,0,0),3);
		s.str("");
	}
	imshow("Blobs Image",color_img);
	waitKey();
	return 0;
}