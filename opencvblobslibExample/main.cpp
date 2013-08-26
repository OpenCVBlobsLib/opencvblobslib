#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "blob.h"
#include "BlobResult.h"
#include <intrin.h>

const int IMSIZE = 400;
const int NUMCORES = 2;
using namespace std;


//iter = iterations for every resolution, in order to be able to compute a mean computation time.
void testTimes(int startRes,int endRes, int step,string fileName,int iter=1);

void test();
void opencvLogo();
void testJoin();

int main(){
	//testTimes(500,6000,250,"Tempi",5);
	opencvLogo();
	//test();
	//testJoin();
	return 0;
}

void testTimes(int startRes,int endRes, int step,string fileName, int iter){
	ofstream fileOutST(fileName+"ST.txt");
	ofstream fileOutMT(fileName+"MT.txt");
	ofstream fileOutInfo(fileName+"Info.txt");
	char CPUBrandString[49];
	__cpuid((int*)CPUBrandString, 0x80000002);
	__cpuid((int*)(CPUBrandString+16), 0x80000003);
	__cpuid((int*)(CPUBrandString+32), 0x80000004);
	CPUBrandString[48] = 0;
	fileOutInfo << CPUBrandString;
	int64 time;
	double elapsed;
	RNG random;
	Mat color_img = imread("testImage.png");
	Mat temp_color_img;
	CBlobResult res;
	cvtColor(color_img,color_img,CV_BGR2GRAY);
	for(int i=0;i<=(endRes-startRes)/step;i++){
		int resolution = step*i+startRes;
		resize(color_img,temp_color_img,Size(resolution,resolution),0,0,INTER_LINEAR);
		threshold(temp_color_img,temp_color_img,250,255,CV_THRESH_BINARY_INV);
		cout <<"RISOLUZIONE: " << resolution<<"x"<<resolution<<endl;
		fileOutST << resolution;
		for(int j=0;j<iter;j++){
			time=getTickCount();
			res = CBlobResult(&(IplImage)temp_color_img,NULL,0);
			elapsed =  (getTickCount()-time)/getTickFrequency();
			cout <<"Tempo Single Thread: " <<elapsed<<endl;
			fileOutST <<"\t" << elapsed;
		}
		fileOutST << "\n";
		fileOutMT << resolution;
		for(int j=0;j<iter;j++){
			time=getTickCount();
			res = CBlobResult(temp_color_img,Mat(),0,NUMCORES);
			elapsed =  (getTickCount()-time)/getTickFrequency();
			cout <<"Tempo Multi Thread: " <<elapsed<<endl;
			fileOutMT <<"\t" << elapsed;
		}
		fileOutMT << "\n";
	}
	fileOutST.close();
	fileOutMT.close();
	fileOutInfo.close();
	cout <<"Premere un tasto per continuare...."<<endl;
	cin.get();
}
void test()
{
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
	cout <<"found: "<<blobs.GetNumBlobs()<<endl;
	cout <<"Tempo ST: "<<(getTickCount() -time)/getTickFrequency()<<endl;
	for(int i=0;i<blobs.GetNumBlobs();i++){
		blobs.GetBlob(i)->FillBlob(color_img,CV_RGB(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255)));
	}
	displayOverlay("Blobs Image","Single Thread");
	imshow("Blobs Image",color_img);
	waitKey();
	time=getTickCount();
	blobs = CBlobResult(binary_img,Mat(),0,NUMCORES);
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
		// 		CvSeqReader reader;
		// 		Point pt;
		// 		if(curblob->GetExternalContour()->GetContourPoints()){
		// 			cvStartReadSeq(curblob->GetExternalContour()->GetContourPoints(),&reader);
		// 			for(int j=0;j<curblob->GetExternalContour()->GetContourPoints()->total;j++){
		// 				CV_READ_SEQ_ELEM(pt,reader);
		// 				color_img.at<Vec3b>(pt) = color;		
		// 			}
		// 		}
		s<<i;
		putText(color_img,s.str(),curblob->getCenter(),1.6,IMSIZE/200,CV_RGB(200,200,200),3);
		s.str("");
		// 		displayOverlay("Blobs Image","Press a key to show the next blob",500);
		// 		imshow("Blobs Image",color_img);
		// 		waitKey();
	}
	displayOverlay("Blobs Image","Multi Thread");
	imshow("Blobs Image",color_img);
	waitKey();
}
void opencvLogo()
{
	Mat im = imread("opencvblobslibBIG.png");
	Mat img;
	cvtColor(im,img,CV_BGR2GRAY);
	threshold(img,img,254,255,CV_THRESH_BINARY_INV);
	int64 time = getTickCount();
	CBlobResult res(img,Mat(),0,NUMCORES);
	cout << "Tempo: " << (getTickCount() - time)/getTickFrequency();
	stringstream ss;
	for(int i=0;i<res.GetNumBlobs();i++){
		ss << i;
		rectangle(im,res.GetBlob(i)->GetBoundingBox(),Scalar(20,200,20),2);
		putText(im,ss.str(),res.GetBlob(i)->getCenter(),1,1,Scalar(40,200,255),1,8);
		ss.str("");
	}
	imshow("Logo",im);
	waitKey();
	imwrite("Logo.png",im);
}
void testJoin(){
	Mat im = imread("opencvblobslibBIG.png");
	Mat img,imt,im2;
	cvtColor(im,img,CV_BGR2GRAY);
	threshold(img,imt,254,255,CV_THRESH_BINARY_INV);
	CBlobResult res(imt,Mat(),0,1);
	CBlob temp,t2;
	for(int i=0;i<res.GetNumBlobs();i++){
		im2 = im.clone();
		temp.JoinBlob(res.GetBlob(i));
		t2 = CBlob(temp);
		Rect bbox = t2.GetBoundingBox();
		rectangle(im2,bbox,Scalar(0,220,0),3);
		t2.FillBlob(im2,Scalar(200,200,0));
		cout << "Perimeter: " << t2.Perimeter();
		cout << "\tArea: " << t2.Area();
		cout << endl;
		cout << "ExtPerim: " << t2.ExternPerimeter(Mat());
		cout << "\tNBlobs: " << t2.getNumJoinedBlobs();
		cout << endl;
		cout << "Mean: " << t2.Mean(img);
		cout << "\tM00: " << t2.Moment(0,0);
		cout << endl;
		imshow("temp",im2);
		waitKey();
	}
	cout << t2.getNumJoinedBlobs() << endl;
	
}

