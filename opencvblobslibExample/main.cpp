#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "blob.h"
#include "BlobResult.h"

const int NUMCORES = 2;
using namespace std;


//iter = iterations for every resolution, in order to be able to compute a mean computation time.
//void testTimes(int startRes,int endRes, int step,string fileName,int iter=1);

void test();
void opencvLogo();
void testJoin();
void testMio();

int main(){
//  	testTimes(500,3000,250,"TempiHR",20);
  	//testTimes(1000,1000,20,"TempiLR",40);
	//testTimes(10,2000,50,"TempiRandom",15);
	//opencvLogo();
	//test();
	testJoin();
	//testMio();
	cout <<"Press a key to continue..."<<endl;
	cin.get();
	return 0;
}

/*void testTimes(int startRes,int endRes, int step,string fileName, int iter){
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
		temp_color_img = color_img.clone();
		resize(color_img,temp_color_img,Size(resolution,resolution),0,0,INTER_NEAREST);
		threshold(temp_color_img,temp_color_img,250,255,CV_THRESH_BINARY_INV);
  		//namedWindow("image",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
  		//imshow("image",temp_color_img);
  		//waitKey();
		cout <<"RISOLUZIONE: " << resolution<<"x"<<resolution<<endl;
		fileOutST << resolution;
		Mat mt;
 		for(int j=0;j<iter;j++){
 			time=getTickCount();
 			CBlobResult res(temp_color_img,mt,1);
 			elapsed =  (getTickCount()-time)/getTickFrequency();
 			cout <<j<<"/"<<iter<<"\tTempo Single Thread: " <<elapsed<<"\t Nblobs: "<<res.GetNumBlobs()<<"\tTime: "<<endl;
 			fileOutST <<"\t" << elapsed;
 		}
		fileOutST << "\n";
		fileOutMT << resolution;

		//cout <<"MULTITHREAD"<<endl;
		for(int j=0;j<iter;j++){
			time=getTickCount();
			CBlobResult res(temp_color_img,mt,NUMCORES);
			//CBlobResult res(&(IplImage)temp_color_img,NULL,0,NUMCORES);
			elapsed =  (getTickCount()-time)/getTickFrequency();
			cout <<j<<"/"<<iter<<"\tTempo Multi Thread: " <<elapsed<<"\t Nblobs: "<<res.GetNumBlobs()<<"\tTime: "<<endl;
			fileOutMT <<"\t" << elapsed;
		}
		fileOutMT << "\n";
	}
	fileOutST.close();
	fileOutMT.close();
	fileOutInfo.close();
}*/
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
	IplImage temp = (IplImage)binary_img;
	blobs = CBlobResult(&temp,NULL,1);
	cout <<"found: "<<blobs.GetNumBlobs()<<endl;
	cout <<"Tempo ST: "<<(getTickCount() -time)/getTickFrequency()<<endl;
	for(int i=0;i<blobs.GetNumBlobs();i++){
		blobs.GetBlob(i)->FillBlob(color_img,CV_RGB(random.uniform(0,255),random.uniform(0,255),random.uniform(0,255)));
	}
	displayOverlay("Blobs Image","Single Thread");
	imshow("Blobs Image",color_img);
	waitKey();
	time=getTickCount();
	blobs = CBlobResult(binary_img,Mat(),NUMCORES);
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
		putText(color_img,s.str(),curblob->getCenter(),1,binary_img.size().width/400,CV_RGB(200,200,200),3);
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
	CBlobResult res(img,Mat(),NUMCORES);
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
	CBlobResult res(imt,Mat(),NUMCORES);
	im2 = im.clone();
	stringstream s;
	for(int i=0;i<res.GetNumBlobs();i++){
		double mean, stddev;
		CBlob t2 = res.GetBlob(i);
		Rect bbox = t2.GetBoundingBox();
		rectangle(im2,bbox,Scalar(0,220,0),3);
		t2.FillBlob(im2,Scalar(200,200,0),0,0,true,im);
		ellipse(im2,t2.GetEllipse(),Scalar(220,0,0),2);
		t_contours hull;
		t2.GetConvexHull(hull);
		drawContours(im2,hull,-1,Scalar(0,0,220),2);
		t2.MeanStdDev(img,&mean,&stddev);
		cout << "Perimeter: " << t2.Perimeter();
		cout << "\tArea: " << t2.Area();
		cout << endl;
		cout << "ExtPerim: " << t2.ExternPerimeter(Mat());
		cout << "\tNBlobs: " << t2.getNumJoinedBlobs();
		cout << endl;
		cout << "Mean: " << mean;
		cout << "\tStdDev: " << stddev;
		cout << endl;
		cout << "M00: " << t2.Moment(0,0);
		cout << endl;
		cout << "ID: " << t2.GetID();
		cout << endl;
	}
	for(int i=0;i<res.GetNumBlobs();i++){
		CBlob *t2 = res.GetBlob(i);
		s << i;
		putText(im2,s.str(),t2->getCenter(),1,im2.size().width/400,CV_RGB(0,0,0),2);
		s.str("");
	}
	res.PrintBlobs("testFile.txt");
	imshow("temp",im2);
	cout << "======================================="<<endl;
	cout << "Focus on the window and press a key to continue and join blobs"<<endl;
	cout << "======================================="<<endl;
	waitKey();
	/*displayOverlay("temp","Press a key to continue and join blobs.",10);*/
	CBlob temp,t2;
	for(int i=0;i<res.GetNumBlobs();i++){
		double mean, stddev;
		im2 = im.clone();
		temp.JoinBlob(res.GetBlob(i));
		t2 = CBlob(temp);
		Rect bbox = t2.GetBoundingBox();
		rectangle(im2,bbox,Scalar(0,220,0),3);
		t2.FillBlob(im2,Scalar(200,200,0),0,0,true,im);
		ellipse(im2,t2.GetEllipse(),Scalar(220,0,0),2);
		t_contours hull;
		t2.GetConvexHull(hull);
		drawContours(im2,hull,-1,Scalar(0,0,220),2);
		t2.MeanStdDev(img,&mean,&stddev);
		cout << "Perimeter: " << t2.Perimeter();
		cout << "\tArea: " << t2.Area();
		cout << endl;
		cout << "ExtPerim: " << t2.ExternPerimeter(Mat());
		cout << "\tNBlobs: " << t2.getNumJoinedBlobs();
		cout << endl;
		cout << "Mean: " << mean;
		cout << "\tStdDev: " << stddev;
		cout << endl;
		cout << "M00: " << t2.Moment(0,0);
		cout << endl;
		imshow("temp",im2);
		waitKey();
	}
	destroyAllWindows();
	cout << t2.getNumJoinedBlobs() << endl;
	
}

void testMio()
{
	RNG rand;
	int trials=100;
	
	
	//namedWindow("imgST",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
	namedWindow("image",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
	namedWindow("BlobsST",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
	namedWindow("BlobsMT",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
	Mat image = imread("testImage.png",CV_LOAD_IMAGE_GRAYSCALE);
	//resize(image,image,Size(14,14),0,0,INTER_NEAREST);
	Mat imBlobsST(image.size(),CV_8UC3),imBlobsMT(image.size(),CV_8UC3);
	Mat mask = Mat::zeros(image.size(),CV_8UC1);
	imBlobsST.setTo(Vec3b(0,0,0));
	imBlobsMT.setTo(Vec3b(0,0,0));
	threshold(image,image,250,255,CV_THRESH_BINARY_INV);
	double time=0;
	imshow("image",image);
	CBlobResult res;
	for(int i=0;i<trials;i++){
		int64 ticks=getTickCount();
		CBlobResult res2(image,Mat(),1);
		time+= (getTickCount()-ticks)/getTickFrequency();
		res=res2;
		cout << i << "/"<<trials<<endl;
	}
	cout << "TimeBlobNormalST: " << time/trials<<"\tCount: "<<res.GetNumBlobs()<<endl; time=0;
	for(int i=0;i<res.GetNumBlobs();i++){
		res.GetBlob(i)->FillBlob(imBlobsST,CV_RGB(rand.uniform(0,255),rand.uniform(0,255),0),0,0,true);
	}
	imshow("BlobsST",imBlobsST);
	waitKey();
	imBlobsST.setTo(Vec3b(0,0,0));

	//myCompLabeler lbl(image.clone(),Point(0*image.size().width,0),Point(1*image.size().width,image.size().height));
	//myCompLabelerGroup gro;
	//gro.set(2,image.clone());
	//
	//Blob_vector blobs;
 // 	imshow("image",image);
	////waitKey();
 //     for(int i=0;i<trials;i++){
 // 		gro.Reset();
 // 		blobs.clear();
 //      	int64 ticks=getTickCount();
 //      	gro.doLabeling(blobs);
 // 		time+= (getTickCount()-ticks)/getTickFrequency();
 //     }
 //     cout << "TimeMT: " << time/trials<<"\tCount: "<<blobs.size()<<endl;


	//time =0;
	//for(int i=0;i<trials;i++){
	//	lbl.Reset();
	//	int64 ticks=getTickCount();
	//	lbl.Label();
	//	time+= (getTickCount()-ticks)/getTickFrequency();
	//}
	//cout << "TimeST: " << time/trials<<"\tCount: "<<lbl.blobs.size()<<endl;

	//for(int i=0;i<lbl.blobs.size();i++){
	//	lbl.blobs[i]->FillBlob(imBlobsST,CV_RGB(rand.uniform(0,255),rand.uniform(0,255),0),0,0,true);
	//}
	//imshow("BlobsST",imBlobsST);
	//for(int i=0;i<blobs.size();i++){
	//	blobs[i]->FillBlob(imBlobsMT,CV_RGB(rand.uniform(0,255),rand.uniform(0,255),0),0,0,true);
	//}
	//imshow("BlobsMT",imBlobsMT);	
	//waitKey();
	destroyAllWindows();
}

