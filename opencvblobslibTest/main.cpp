#include "BlobResult.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(){
	Mat source = Mat::ones(1600,1600,CV_8UC1)*255;
	Mat out = Mat::zeros(1600,1600,CV_8UC1);
	int64 time;
	namedWindow("Blobs",CV_GUI_NORMAL+CV_WINDOW_NORMAL+CV_WINDOW_KEEPRATIO);
	time = getTickCount();
	CBlobResult res(source,Mat(),0);
	for(int i=0;i<res.GetNumBlobs();i++){
		res.GetBlob(i)->FillBlob(out,Scalar(10*i+10));
	}
	imshow("Blobs",out);
	cout <<"Interfaccia MultiThread: "<<(getTickCount()-time)/getTickFrequency();
	waitKey();

	time = getTickCount();
	res=CBlobResult(&(IplImage)source,NULL,0);
	for(int i=0;i<res.GetNumBlobs();i++){
		res.GetBlob(i)->FillBlob(out,Scalar(10*i+10));
	}
	imshow("Blobs",out);
	cout <<"Interfaccia SingleThread: "<<(getTickCount()-time)/getTickFrequency();
	waitKey();
	//cout <<"Premere un tasto per terminare..."<<endl;
	//cin.get();
	return 0;
}