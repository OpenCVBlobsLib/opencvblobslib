#include "BlobResult.h"
#include <opencv2/opencv.hpp>
#include <pthread.h>

using namespace cv;
using namespace std;

const int IMSIZE = 200;

void testJoin1();
void testJoin2();

int main(){
	testJoin2();
	return 0;
}

void testJoin1()
{
	cout<<"Unsigned int size: "<<sizeof(char);
	Mat source = Mat::ones(IMSIZE,IMSIZE,CV_8UC1)*255;
	Rect roi1 = Rect(IMSIZE/4,0,IMSIZE/2,IMSIZE);
	source(roi1).setTo(0);
	Mat_<Vec3b> out = Mat_<Vec3b>::zeros(IMSIZE,IMSIZE);
	Mat_<Vec3b> outMT = Mat_<Vec3b>::zeros(IMSIZE,IMSIZE);
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
		//cout<<endl<<"Informazioni blob prima join: H "<<res.GetBlob(0)->GetBoundingBox().height<<" W "<<res.GetBlob(0)->GetBoundingBox().height<<endl;
		res.GetBlob(4)->JoinBlob(res.GetBlob(6));
		res.GetBlob(6)->to_be_deleted=true;
		res.GetBlob(2)->JoinBlob(res.GetBlob(4));
		res.GetBlob(4)->to_be_deleted=true;
		res.GetBlob(0)->JoinBlob(res.GetBlob(2));
		res.GetBlob(2)->to_be_deleted=true;

		res.GetBlob(1)->JoinBlob(res.GetBlob(3));
		res.GetBlob(3)->to_be_deleted=true;
		res.GetBlob(1)->JoinBlob(res.GetBlob(5));
		res.GetBlob(5)->to_be_deleted=true;
		res.GetBlob(1)->JoinBlob(res.GetBlob(7));
		res.GetBlob(7)->to_be_deleted=true;
		res.Filter(res,B_EXCLUDE,CBlobGetTBDeleted(),B_EQUAL,1);

		cout<<endl<<"Numero blobs trovati: "<<res.GetNumBlobs()<<endl;
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
}

void testJoin2()
{
	cout<<"Unsigned int size: "<<sizeof(char);
	Mat source = Mat::ones(IMSIZE,IMSIZE,CV_8UC1)*255;
	Rect roi1 = Rect(IMSIZE/4,0,IMSIZE/2,IMSIZE);
	source(roi1).setTo(0);
	Mat_<Vec3b> out = Mat_<Vec3b>::zeros(IMSIZE,IMSIZE);
	Mat_<Vec3b> outMT = Mat_<Vec3b>::zeros(IMSIZE,IMSIZE);
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
		res.GetBlob(4)->JoinBlobTangent(res.GetBlob(6));
		res.GetBlob(2)->JoinBlobTangent(res.GetBlob(4));
		res.GetBlob(0)->JoinBlobTangent(res.GetBlob(2));
		for(int i=0;i<1;i++){
			res.GetBlob(i)->FillBlob(outMT,Scalar(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255)));
			//cvDrawContours(&(IplImage)outMT,res.GetBlob(i)->GetExternalContour()->GetContourPoints(),Scalar(200,200,0),Scalar(),2000,1,8);
			CvSeqReader reader;
			Point pt;
			cvStartReadSeq(res.GetBlob(i)->GetExternalContour()->GetContourPoints(),&reader);
			for(int j=0;j<res.GetBlob(i)->GetExternalContour()->GetContourPoints()->total;j++){
				CV_READ_SEQ_ELEM(pt,reader);
				outMT.at<Vec3b>(pt) = Vec3b(200,200,0);
			}
			//cout << Rect(res.GetBlob(i)->GetBoundingBox())<<endl;
			//rectangle(outMT,res.GetBlob(i)->GetBoundingBox(),Scalar(0,200,0),IMSIZE/160);
			s<<i;
			putText(outMT,s.str(),res.GetBlob(i)->getCenter(),1.6,IMSIZE/160,CV_RGB(255,255,255),3);
			s.str("");
			vector<vector<Point>> seg = res.GetBlob(i)->getPointsTouchingBorder(0);
			for(int j=0;j< seg.size();j++){
				for(int k=0;k<seg[j].size();k++){
					outMT.at<Vec3b>(seg[j][k]) = Vec3b(200,40,80);
				}
			}
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
	waitKey();
	//cout <<"Premere un tasto per terminare..."<<endl;
	//cin.get();
}
