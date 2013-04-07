#include "BlobResult.h"
#include <opencv2/opencv.hpp>
#include <pthread.h>

using namespace cv;
using namespace std;

int main(){
	cout<<"Unsigned int size: "<<sizeof(char);
	Mat source = Mat::ones(3200,3200,CV_8UC1)*255;
	Rect roi1 = Rect(800,0,1600,3200);
	source(roi1).setTo(0);
	Mat_<Vec3b> out = Mat_<Vec3b>::zeros(3200,3200);
	Mat_<Vec3b> outMT = Mat_<Vec3b>::zeros(3200,3200);
	double medST=0,medMT=0;
	int64 time;
	double elapsed;
	namedWindow("BlobsMT",CV_GUI_NORMAL+CV_WINDOW_NORMAL+CV_WINDOW_KEEPRATIO);
	namedWindow("BlobsST",CV_GUI_NORMAL+CV_WINDOW_NORMAL+CV_WINDOW_KEEPRATIO);
	moveWindow("BlobsMT",300,200);
	moveWindow("BlobsST",700,200);
	RNG random;
	CBlobResult res;
	stringstream s;
	for(int j=0;j<10;j++){
		time = getTickCount();
		res=CBlobResult(source,Mat(),0);
		elapsed = (getTickCount()-time)/getTickFrequency();
		cout<<endl<<"NumblobsMT: "<<res.GetNumBlobs()<<endl;
		cout <<"Interfaccia MultiThread: "<<elapsed<<endl;
		medMT+=elapsed;

		cout<<endl<<"Informazioni blob prima join: H "<<res.GetBlob(0)->GetBoundingBox().height<<" W "<<res.GetBlob(0)->GetBoundingBox().height<<endl;
		res.GetBlob(0)->JoinBlob(res.GetBlob(2));
		res.GetBlob(0)->JoinBlob(res.GetBlob(4));
		cout<<endl<<"Informazioni blob dopo join: H "<<res.GetBlob(0)->GetBoundingBox().height<<" W "<<res.GetBlob(0)->GetBoundingBox().height<<endl;
		//res.PrintBlobs("prova.txt");

		for(int i=0;i<res.GetNumBlobs();i++){
			res.GetBlob(i)->FillBlob(outMT,Scalar(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255)));
			rectangle(outMT,res.GetBlob(i)->GetBoundingBox(),Scalar(0,200,0),10);
			s<<i;
			putText(outMT,s.str(),Point(res.GetBlob(i)->GetBoundingBox().x+res.GetBlob(i)->GetBoundingBox().width*0.5,res.GetBlob(i)->GetBoundingBox().y+res.GetBlob(i)->GetBoundingBox().height*0.5),1.6,10,CV_RGB(255,255,255),3);
			s.str("");
		}
		imshow("BlobsMT",outMT);
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
		res=CBlobResult(&(IplImage)source,NULL,0);
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