#include "BlobResult.h"
#include <opencv2/opencv.hpp>
#include <pthread.h>

using namespace cv;
using namespace std;

int main(){
	Mat source = Mat::ones(2*1600,2*1600,CV_8UC1)*255;
	Mat_<Vec3b> out = Mat_<Vec3b>::zeros(2*1600,2*1600);
	double medST=0,medMT=0;
	int64 time;
	double elapsed;
	namedWindow("BlobsMT",CV_GUI_NORMAL+CV_WINDOW_NORMAL+CV_WINDOW_KEEPRATIO);
	namedWindow("BlobsST",CV_GUI_NORMAL+CV_WINDOW_NORMAL+CV_WINDOW_KEEPRATIO);
	moveWindow("BlobsMT",300,200);
	moveWindow("BlobsST",700,200);
	RNG random;
	CBlobResult res;
	for(int j=0;j<10;j++){
		time = getTickCount();
		res=CBlobResult(source,Mat(),0);
		elapsed = (getTickCount()-time)/getTickFrequency();
		cout <<"Interfaccia MultiThread: "<<elapsed;
		medMT+=elapsed;
		for(int i=0;i<res.GetNumBlobs();i++){
			res.GetBlob(i)->FillBlob(out,Scalar(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255)));
			rectangle(out,res.GetBlob(i)->GetBoundingBox(),Scalar(0,200,0),10);
		}
		imshow("BlobsMT",out);
		waitKey(1);

		out.setTo(0);

		time = getTickCount();
		res=CBlobResult(&(IplImage)source,NULL,0);
		elapsed = (getTickCount()-time)/getTickFrequency();
		cout <<endl<<"Interfaccia SingleThread: "<<elapsed<<endl;
		medST+=elapsed;
		for(int i=0;i<res.GetNumBlobs();i++){
			res.GetBlob(i)->FillBlob(out,Scalar(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255)));
			rectangle(out,res.GetBlob(i)->GetBoundingBox(),Scalar(0,200,0),10);
		}
		imshow("BlobsST",out);
		waitKey(1);
	}
	medST/=10;
	medMT/=10;
	cout <<endl;
	cout << "Tempo MEDIO MultiThread: "<<medMT<<endl;
	cout << "Tempo MEDIO SingleThread: "<<medST<<endl;
	cout <<endl;
	out.setTo(0);
	Rect roi;
	Size sz = source.size();
	int numCores = pthread_num_processors_np();
	roi = Rect(0,0,sz.width,sz.height/numCores);
	cout<<endl<<"rows "<<sz.height/numCores;

	time = getTickCount();
	res=CBlobResult(&(IplImage)(source(roi)),NULL,0);
	elapsed = (getTickCount()-time)/getTickFrequency();
	cout <<endl<<"Interfaccia SingleThread con una sola sezione dell'immagine: "<<elapsed<<endl;
	for(int i=0;i<res.GetNumBlobs();i++){
		res.GetBlob(i)->FillBlob(out,Scalar(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255)));
	}
	imshow("BlobsST",out);
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