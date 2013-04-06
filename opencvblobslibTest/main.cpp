#include "BlobResult.h"
#include <opencv2/opencv.hpp>
#include <pthread.h>

using namespace cv;
using namespace std;

int main(){
	Mat source = Mat::ones(2*1600,2*1600,CV_8UC1)*255;
	Mat out = Mat::zeros(2*1600,2*1600,CV_8UC1);
	int64 time;
	namedWindow("Blobs",CV_GUI_NORMAL+CV_WINDOW_NORMAL+CV_WINDOW_KEEPRATIO);
	time = getTickCount();
	CBlobResult res(source,Mat(),0);
	for(int i=0;i<res.GetNumBlobs();i++){
		res.GetBlob(i)->FillBlob(out,Scalar(10*i+10));
	}
	double elapsed = (getTickCount()-time)/getTickFrequency();
	imshow("Blobs",out);
	cout <<"Interfaccia MultiThread: "<<elapsed;
	waitKey();

	time = getTickCount();
	res=CBlobResult(&(IplImage)source,NULL,0);
	for(int i=0;i<res.GetNumBlobs();i++){
		res.GetBlob(i)->FillBlob(out,Scalar(10*i+10));
	}
	elapsed = (getTickCount()-time)/getTickFrequency();
	imshow("Blobs",out);
	cout <<endl<<"Interfaccia SingleThread: "<<elapsed<<endl;
	waitKey();

	Rect roi;
	Size sz = source.size();
	int numCores = pthread_num_processors_np();
	roi = Rect(0,0,sz.width,sz.height/numCores);
	cout<<endl<<"rows "<<sz.height/numCores;

	time = getTickCount();
	res=CBlobResult(&(IplImage)(source(roi)),NULL,0);
	for(int i=0;i<res.GetNumBlobs();i++){
		res.GetBlob(i)->FillBlob(out(roi),Scalar(10*i+10));
	}
	elapsed = (getTickCount()-time)/getTickFrequency();
	imshow("Blobs",out);
	cout <<endl<<"Interfaccia SingleThread con una sola sezione dell'immagine: "<<elapsed<<endl;
	waitKey();

	time = getTickCount();
	for(int i = 0;i<4;i++){
		Mat mask = Mat_<uchar>::zeros(source.size());
		Rect roi;
		Size sz = source.size();
		Rect(0,0,sz.width,sz.height/numCores);
		mask(roi).setTo(255);
		mask.clone();
		mask.setTo(0);
	}
	elapsed = (getTickCount()-time)/getTickFrequency();
	cout <<endl<<"4 volte creazione maschera: "<<elapsed<<endl;
	waitKey();
	//cout <<"Premere un tasto per terminare..."<<endl;
	//cin.get();
	return 0;
}