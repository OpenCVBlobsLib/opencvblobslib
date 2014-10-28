#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "blob.h"
#include "BlobResult.h"
#include "highgui.h" //include it to use GUI functions.

const int NUMCORES = 2;
using namespace std;
using namespace cv;


//iter = iterations for every resolution, in order to be able to compute a mean computation time.
//void testTimes(int startRes,int endRes, int step,string fileName,int iter=1);

void test();
void opencvLogo();
void testJoin();
void testRandomImage(); //Generate a random image and test blobs.

int main(){
//  	testTimes(500,3000,250,"TempiHR",20);
  	//testTimes(1000,1000,20,"TempiLR",40);
	//testTimes(10,2000,50,"TempiRandom",15);
	//opencvLogo();
	//test();
	testJoin();
	//testMio();
	//testRandomImage();

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

	rectangle(color_img,blobs.getBlobNearestTo(Point(color_img.size().width/2,color_img.size().height/2))->GetBoundingBox(),CV_RGB(200,100,50),1);

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
	//imt = imt(Rect(30,50,100,100));
	imshow("binImage",imt);
	CBlobResult res(imt,Mat(),NUMCORES);
	im2 = im.clone();
	//im2 = im(Rect(30,50,100,100)).clone();
	stringstream s;
	for(int i=0;i<res.GetNumBlobs();i++){
		Scalar mean, stddev;
		CBlob t2 = res.GetBlob(i);
		Rect bbox = t2.GetBoundingBox();
		rectangle(im2,bbox,Scalar(0,220,0),1);
		t2.FillBlob(im2,Scalar(200,200,0),0,0,true,im);
		ellipse(im2,t2.GetEllipse(),Scalar(220,0,0),2);
		t_contours hull;
		t2.GetConvexHull(hull);
		drawContours(im2,hull,-1,Scalar(0,0,220),2);
		t2.MeanStdDev(im,mean,stddev);
		cout << "Perimeter: " << t2.Perimeter();
		cout << endl;
		cout << "AreaPixel: " << t2.Area(PIXELWISE);
		cout << "\tAreaGreen: " << t2.Area(GREEN);
		cout << endl;
		cout << "ExtPerim: " << t2.ExternPerimeter(Mat());
		cout << "\tNBlobs: " << t2.getNumJoinedBlobs();
		cout << endl;
		cout << "Mean: " << mean;
		cout << "\tStdDev: " << stddev;
		cout << endl;
		cout << "DensityPixel: " << t2.density(PIXELWISE);
		cout << "\tDensityGreen: " << t2.density(GREEN);
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
	
	//Test Overlapping
	//The entire part before the call to overlappingPixels is just for visualisation purposes!
	cout << "======================================="<<endl;
	cout << "Overlapping blobs test"<<endl;
	cout << "======================================="<<endl;
	CBlob *b1 = res.GetBlob(1);
	CBlob *b2 = res.GetBlob(1);
	b2->ShiftBlob(-30,0); //The shift is just to create an actual overlap between the first 2 blobs, when using 2 threads!
	Rect r1 = b1->GetBoundingBox(),r2 = b2->GetBoundingBox();
	Rect minContainingRect = r1 | r2; //Minimum containing rectangle
	Mat m1 = Mat::zeros(minContainingRect.height,minContainingRect.width,CV_8UC3);
	Mat m2 = Mat::zeros(minContainingRect.height,minContainingRect.width,CV_8UC3);
	b1->FillBlob(m1,Scalar(100,0,0),-minContainingRect.x,-minContainingRect.y,true);
	b2->FillBlob(m2,Scalar(0,100,0),-minContainingRect.x,-minContainingRect.y,true);
	imshow("Overlap",m1+m2);
	cout << "Overlapping pixels: "<<b1->overlappingPixels(b2)<<endl; //Actual call that computes pixels
	cout << "Focus on the window and press a key to continue to the next test"<<endl;
	cout << endl<<endl<<endl;
	b2->ShiftBlob(30,0); //To reset the state as it was before
	waitKey();
	//

	//Example of filter using (I have to delete some blobs in the previous cycle though!
	//res.Filter(res,FilterAction::FLT_EXCLUDE,CBlobGetTBDeleted(),FilterCondition::FLT_EQUAL,1);
	//
	res.PrintBlobs("testFile.txt");
	imshow("temp",im2);
	//Test Overlapping
	cout << "======================================="<<endl;
	cout << "Blob joining test"<<endl;
	cout << "Focus on the window and press a key to continue and join blobs"<<endl;
	cout << "======================================="<<endl;
	waitKey();
	/*displayOverlay("temp","Press a key to continue and join blobs.",10);*/
	CBlob temp,t2;
	for(int i=0;i<res.GetNumBlobs();i++){
		Scalar mean, stddev;
		im2 = im.clone();
		temp.JoinBlob(res.GetBlob(i));
		t2 = CBlob(temp);
		Rect bbox = t2.GetBoundingBox();
		rectangle(im2,bbox,Scalar(0,220,0),1);
		t2.FillBlob(im2,Scalar(200,200,0),0,0,true,im);
		ellipse(im2,t2.GetEllipse(),Scalar(220,0,0),2);
		t_contours hull;
		t2.GetConvexHull(hull);
		drawContours(im2,hull,-1,Scalar(0,0,220),2);
		t2.MeanStdDev(im,mean,stddev);
		cout << "Perimeter: " << t2.Perimeter();
		cout << endl;
		cout << "AreaPixel: " << t2.Area(PIXELWISE);
		cout << "\tAreaGreen: " << t2.Area(GREEN);
		cout << endl;
		cout << "ExtPerim: " << t2.ExternPerimeter(Mat());
		cout << "\tNBlobs: " << t2.getNumJoinedBlobs();
		cout << endl;
		cout << "Mean: " << mean;
		cout << "\tStdDev: " << stddev;
		cout << endl;
		cout << "DensityPixel: " << t2.density(PIXELWISE);
		cout << "\tDensityGreen: " << t2.density(GREEN);
		cout << endl;
		cout << "M00: " << t2.Moment(0,0);
		cout << endl;
		imshow("temp",im2);
		waitKey();
	}
	destroyAllWindows();
	cout << t2.getNumJoinedBlobs() << endl;
	
}

void testRandomImage()
{
	cout << "Random test: Press esc to quit"<<endl;
	namedWindow("Binary Image",CV_WINDOW_NORMAL);
	namedWindow("Blobs Image",CV_WINDOW_NORMAL);
	Mat image(256,256,CV_8UC1),nulMask;
	Mat out(image.size(),CV_8UC3);
	RNG rand(0);
	while(true){
		out.setTo(0);
		rand.fill(image,CV_8UC1,0,256);
		threshold(image,image,127,255,CV_THRESH_BINARY);
		CBlobResult res(image,nulMask,2);
		cout << "Nblobs: "<< res.GetNumBlobs()<<endl;
		for(int i=0;i<res.GetNumBlobs();i++){
			CBlob *blob = res.GetBlob(i);
			Scalar color(rand.uniform(0,256),rand.uniform(0,256),rand.uniform(0,256));
			blob->FillBlob(out,color,0,0,true);
			Point s = blob->GetExternalContour()->GetStartPoint();
			out.at<Vec3b>(s) = Vec3b(255,255,255);
		}
		imshow("Binary Image",image);
		imshow("Blobs Image",out);
		int k = waitKey(1);
		if(k==27)
			break;
	}
	destroyAllWindows();
}

