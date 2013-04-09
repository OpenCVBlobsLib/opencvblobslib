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
	cout<<"found: "<<blobs.GetNumBlobs()<<endl;
	stringstream s;
	cout << "Start 0: " << Point(blobs.GetBlob(0)->GetExternalContour()->GetStartPoint())<<endl;
	cout << "Start 5: " << Point(blobs.GetBlob(5)->GetExternalContour()->GetStartPoint())<<endl;
	cout << "Start 4: " << Point(blobs.GetBlob(4)->GetExternalContour()->GetStartPoint())<<endl;
	blobs.GetBlob(0)->JoinBlobTangent(blobs.GetBlob(5));
	blobs.GetBlob(0)->JoinBlobTangent(blobs.GetBlob(4));
	blobs.GetBlob(8)->JoinBlobTangent(blobs.GetBlob(16));
	blobs.GetBlob(3)->JoinBlobTangent(blobs.GetBlob(8));
	blobs.GetBlob(6)->JoinBlobTangent(blobs.GetBlob(15));
	blobs.GetBlob(2)->JoinBlobTangent(blobs.GetBlob(6));
	blobs.GetBlob(1)->JoinBlobTangent(blobs.GetBlob(2));
	int numBlobs = blobs.GetNumBlobs();
	color_img.setTo(Vec3b(0,0,0));
	
	for(int i=0;i<numBlobs;i++){
		if(i != 0 && i != 3 && i != 1)
			continue;
		Vec3b color = Vec3b(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255));
		curblob=blobs.GetBlob(i);
		curblob->FillBlob(color_img,CV_RGB(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255)));
		/*CvSeqReader reader;
		Point pt;
		cvStartReadSeq(curblob->GetExternalContour()->GetContourPoints(),&reader);
		for(int j=0;j<curblob->GetExternalContour()->GetContourPoints()->total;j++){
			CV_READ_SEQ_ELEM(pt,reader);
			color_img.at<Vec3b>(pt) = color;
		}*/
		s<<i;
		putText(color_img,s.str(),curblob->getCenter(),1.6,5,CV_RGB(200,200,200),3);
		s.str("");
	}
	imshow("Blobs Image",color_img);
	waitKey();
	return 0;
}