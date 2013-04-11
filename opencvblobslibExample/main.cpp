#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "blob.h"
#include "BlobResult.h"

const int IMSIZE = 2000;
using namespace std;
int main(){
	int64 time;
	RNG random;
	Mat color_img = imread("opencvblobslibBIG.png");
 	//resize(color_img,color_img,Size(IMSIZE,IMSIZE),0,0,INTER_LINEAR);
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
	CBlobResult blobs;
	color_img.setTo(Vec3b(0,0,0));
	time=getTickCount();
	blobs = CBlobResult(&(IplImage)binary_img,NULL,0);
	cout <<"Tempo ST: "<<(getTickCount() -time)/getTickFrequency()<<endl;
	for(int i=0;i<blobs.GetNumBlobs();i++){
		blobs.GetBlob(i)->FillBlob(color_img,CV_RGB(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255)));
	}
	displayOverlay("Blobs Image","Single Thread");
	imshow("Blobs Image",color_img);
	waitKey();
	time=getTickCount();
	blobs = CBlobResult(binary_img,Mat(),0);
	cout <<"Tempo MT: "<<(getTickCount() -time)/getTickFrequency()<<endl;
	CBlob *curblob;
	cout<<"found: "<<blobs.GetNumBlobs()<<endl;
	stringstream s;
	int numBlobs = blobs.GetNumBlobs();
	color_img.setTo(Vec3b(0,0,0));
	for(int i=0;i<numBlobs;i++){
		Vec3b color = Vec3b(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255));
		curblob=blobs.GetBlob(i);
		//cout <<"Blob "<<i<<": "<< curblob->GetID()<<endl;
		curblob->FillBlob(color_img,CV_RGB(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255)));
		CvSeqReader reader;
		Point pt;
		if(curblob->GetExternalContour()->GetContourPoints()){
			cvStartReadSeq(curblob->GetExternalContour()->GetContourPoints(),&reader);
			for(int j=0;j<curblob->GetExternalContour()->GetContourPoints()->total;j++){
				CV_READ_SEQ_ELEM(pt,reader);
				color_img.at<Vec3b>(pt) = color;		
			}
		}
		s<<i;
		putText(color_img,s.str(),curblob->getCenter(),1.6,IMSIZE/200,CV_RGB(200,200,200),3);
		s.str("");
	}
	displayOverlay("Blobs Image","Multi Thread");
	imshow("Blobs Image",color_img);
	waitKey();
	return 0;
}