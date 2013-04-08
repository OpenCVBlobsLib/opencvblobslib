#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "blob.h"
#include "BlobResult.h"

int main(){
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
	for(int i=0;i<blobs.GetNumBlobs();i++){

	}
	waitKey();
	return 0;
}