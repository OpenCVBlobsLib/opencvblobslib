
#include "ComponentLabeling.h"

myCompLabeler::myCompLabeler(Mat &binImage,Point start,Point end, Mat &lab):
	startPoint(start),endPoint(end),
	labels(lab),
	binaryImage(binImage)
{
	if(labels.empty()){
		labels.create(binImage.size());
		labels.setTo(-1);
	}
	r=0;c=0;
	dir=0;
}

myCompLabeler::~myCompLabeler()
{

}

void myCompLabeler::Reset()
{
	labels.setTo(0);
	blobs.clear();
}

void myCompLabeler::Label()
{
	ptrDataBinary = binaryImage.data;
	ptrDataLabels = (int*)labels.data;

	h = binaryImage.size().height;
	w = binaryImage.size().width;
	int label=-1;
	for(r=startPoint.y;r<endPoint.y;r++){
		//First col
		pos = r*w;
		c=startPoint.x;
		if(ptrDataBinary[pos]){
			label = ptrDataLabels[pos];
			if(label!=0)
				currentBlob = (CBlob *)label;
			if(ptrDataBinary[pos] && ptrDataLabels[pos]==0){
				currentBlob = new CBlob(currentLabel,Point(c,r),Size(w,h));
				blobs.push_back(currentBlob);
				TracerExt();
			}
			if(!ptrDataBinary[pos+1] && ptrDataLabels[pos+1]==0){
				TracerInt();
			}
		}
		//Other cols
		for(c=startPoint.x+1;c<endPoint.x-1;c++){
			pos = r*w+c;
			if(ptrDataBinary[pos]){
				label = ptrDataLabels[pos];
				if(label!=0)
					currentBlob = (CBlob *)label;
				else if(!ptrDataBinary[pos-1]){
					currentBlob = new CBlob(currentLabel,Point(c,r),Size(w,h));
					blobs.push_back(currentBlob);
					TracerExt();
				}
 				if(!ptrDataBinary[pos+1] && ptrDataLabels[pos+1]==0){
       				TracerInt();
 				}
			}
		}
		pos = r*w+c;
		//Last column
		if(ptrDataBinary[pos]){
			label = ptrDataLabels[pos];
			if(label!=0)
				currentBlob = (CBlob *)label;
			else if(!ptrDataBinary[pos-1]){
				currentBlob = new CBlob(currentLabel,Point(c,r),Size(w,h));
				blobs.push_back(currentBlob);
				TracerExt();
			}
		}
	}
}

void* myCompLabeler::thread_Labeling( myCompLabeler* o )
{
	o->Label();
	return 0;
}


void myCompLabeler::TracerExt()
{
	//Dir:
	//321
	//4-0
	//567
 	//cout << "TracerExt\t" << col<<","<<row<<endl;
 	//bool debugDraw=true;

	int sR=r,sC=c;
	int startPos = sR*w+sC;
	dir=6;
  	getNextPointCCW();
 	if(singlePixBlob){
 		r=sR;
 		c=sC;
 		pos = r*w+c;
		ptrDataLabels[pos] = int(currentBlob);
 		return;
 	}
	//ptrDataBinary[pos]= 150;
	t_chainCodeList *cont = &currentBlob->m_externalContour.m_contour[0];
	while(pos!=startPos){
 		//cout << r << "," << c << endl;
		//ptrDataBinary[pos]= 150;
		cont->push_back(dir);
		ptrDataLabels[pos] = (int)currentBlob;
		getNextPointCCW();
   // 	if(debugDraw){
   // 		namedWindow("im",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
   // 		namedWindow("lab",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
   // 		imshow("im",binaryImage);
   // 		imshow("lab",labels);
 		//stringstream ss;
 		//ss << c << "," << r;
 		//displayOverlay("im",ss.str());
   // 		int k = waitKey();
   // 		if(k==' ')
   // 			debugDraw=false;
   // 	}	
	}
	cont->push_back(dir);
	ptrDataLabels[pos] = int(currentBlob);
	
	for(int i=0;i<3;i++){
		getNextPointCCW();
		if(ptrDataLabels[pos]==0){
			while(pos!=startPos){
				//cout << r << "," << c << endl;
				ptrDataLabels[pos] = (int)currentBlob;
				cont->push_back(dir);
				getNextPointCCW();
				// 		ptrDataBinary[pos]= 150;
				// 		if(debugDraw){
				// 			namedWindow("im",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
				// 			namedWindow("lab",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
				// 			imshow("im",binaryImage);
				// 			imshow("lab",labels);
				// 			int k = waitKey();
				// 			if(k==' ')
				// 				debugDraw=false;
				// 		}

			}
			cont->push_back(dir);
			ptrDataLabels[pos] = int(currentBlob);
		}
		else{
			r=sR;
			c=sC;
			pos = r*w+c;
		}
	}
}
void myCompLabeler::TracerInt()
{
	//Dir:
	//321
	//4-0
	//567
	//cout << "TracerInt\t" << col<<","<<row<<endl;
	//bool debugDraw=true;
	
	int startPos = r*w+c;
	CBlobContour* contour = new CBlobContour(Point(c,r),Size(w,h));
	t_chainCodeList *cont = &contour->m_contour[0];
	dir=6;
	getNextPointCW();
	ptrDataLabels[pos] = (int)currentBlob;
	cont->push_back(dir);
	while(pos!=startPos){
		// 		cout << r << "," << c << endl;
		getNextPointCW();
		ptrDataLabels[pos] = (int)currentBlob;
		cont->push_back(dir);
		//contour->AddChainCode(dir);
		//ptrDataBinary[pos] = 100;
		
//  		if(debugDraw){
//  		 	namedWindow("im",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
// 			namedWindow("lab",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
//  		 	imshow("im",binaryImage);
// 			imshow("lab",labels);
// 			stringstream ss;
// 			ss << c << "," << r;
// 			displayOverlay("im",ss.str());
//  		 	int k = waitKey();
//  		 	if(k==' ')
//  		 		debugDraw=false;
//  		}
		
	}
	currentBlob->m_internalContours.push_back(contour);
}

void myCompLabeler::getNextPointCCW()
{
	//Dir:
	//321
	//4-0
	//567

	dir= (dir+6)%8;
	int i=0;
	for(i;i<8;i++,dir = (dir+1)%8){
		tempR = r+freemanR[dir];
		tempC = c+freemanC[dir];
 		if(!(tempR<0 || tempR>=h || tempC<0 || tempC>=w)){
			pos = tempR*w +tempC;
			if(ptrDataBinary[pos]){
				r=tempR;
				c=tempC;
				break;
			}
			ptrDataLabels[pos] = (int)currentBlob;
		}
	}
	singlePixBlob = i == 8;
	pos = r*w+c;

	/*
	// I tried to pre-create all the various direction vectors in order to avoid checking too many times for the boundary of the image.
	// However, there was no gain in performance.
	//It's useless (in terms of performance) to create the direction vectors according to all cases.
	8 directions

	cases:
	0-r=0
	1-r=0,c=0
	2-r=0,c=w
	3-r=h
	4-r=h,c=0
	5-r=h,c=w
	6-c=0
	7-c=w
	8-otherwise
	total:9
	*/
	
	//static uchar directions[8][9][8] = {{{0,4,5,6,7,0,0,0},{0,6,7,0,0,0,0,0},{5,6,7,0,0,0,0,0},{0,1,2,3,4,0,0,0},{0,1,2,0,0,0,0,0},{2,3,4,0,0,0,0,0},{0,1,2,6,7,0,0,0},{2,3,4,5,6,0,0,0},{0,1,2,3,4,5,6,7}},{{4,5,6,7,0,0,0,0},{6,7,0,0,0,0,0,0},{5,6,7,0,0,0,0,0},{1,2,3,4,0,0,0,0},{1,2,0,0,0,0,0,0},{2,3,4,0,0,0,0,0},{1,2,6,7,0,0,0,0},{2,3,4,5,6,0,0,0},{1,2,3,4,5,6,7,0}},{{4,5,6,7,0,0,0,0},{6,7,0,0,0,0,0,0},{5,6,7,0,0,0,0,0},{2,3,4,0,1,0,0,0},{2,0,1,0,0,0,0,0},{2,3,4,0,0,0,0,0},{2,6,7,0,1,0,0,0},{2,3,4,5,6,0,0,0},{2,3,4,5,6,7,0,1}},{{4,5,6,7,0,0,0,0},{6,7,0,0,0,0,0,0},{5,6,7,0,0,0,0,0},{3,4,0,1,2,0,0,0},{0,1,2,0,0,0,0,0},{3,4,2,0,0,0,0,0},{6,7,0,1,2,0,0,0},{3,4,5,6,2,0,0,0},{3,4,5,6,7,0,1,2}},{{4,5,6,7,0,4,0,0},{6,7,0,0,0,0,0,0},{5,6,7,0,0,0,0,0},{4,0,1,2,3,0,0,0},{0,1,2,0,0,0,0,0},{4,2,3,0,0,0,0,0},{6,7,0,1,2,0,0,0},{4,5,6,2,3,0,0,0},{4,5,6,7,0,1,2,3}},{{5,6,7,0,4,5,0,0},{6,7,0,0,0,0,0,0},{5,6,7,0,0,0,0,0},{0,1,2,3,4,0,0,0},{0,1,2,0,0,0,0,0},{2,3,4,0,0,0,0,0},{6,7,0,1,2,0,0,0},{5,6,2,3,4,0,0,0},{5,6,7,0,1,2,3,4}},{{6,7,0,4,5,6,0,0},{6,7,0,0,0,0,0,0},{6,7,5,0,0,0,0,0},{0,1,2,3,4,0,0,0},{0,1,2,0,0,0,0,0},{2,3,4,0,0,0,0,0},{6,7,0,1,2,0,0,0},{6,2,3,4,5,0,0,0},{6,7,0,1,2,3,4,5}},{{7,0,4,5,6,7,0,0},{7,0,6,0,0,0,0,0},{7,5,6,0,0,0,0,0},{0,1,2,3,4,0,0,0},{0,1,2,0,0,0,0,0},{2,3,4,0,0,0,0,0},{7,0,1,2,6,0,0,0},{2,3,4,5,6,0,0,0},{7,0,1,2,3,4,5,6}}};
	//static uchar nDirs[8][9] = {{5,3,3,5,3,3,5,5,8},{5,3,3,5,3,3,5,5,8},{5,3,3,5,3,3,5,5,8},{5,3,3,5,3,3,5,5,8},{5,3,3,5,3,3,5,5,8},{5,3,3,5,3,3,5,5,8},{5,3,3,5,3,3,5,5,8},{5,3,3,5,3,3,5,5,8}};
	//static uchar* dirVec;
	//int p =-1;
	//if(r==0){
	//	if(c==0)
	//		p=1;
	//	else if(c==(w-1))
	//		p=2;
	//	else
	//		p=0;
	//}
	//else if(r==(h-1)){
	//	if(c==0)
	//		p=4;
	//	else if(c==(w-1))
	//		p=5;
	//	else
	//		p=3;
	//}
	//else if(c==0)
	//	p=6;
	//else if(c==(w-1))
	//	p=7;
	//else
	//	p=8;
	//
	//dirVec = directions[dir][p];
	//int i=0;
	//int d=dir;
	//for(i;i<nDirs[d][p];i++){
	//	tempR = r+freemanR[dirVec[i]];
	//	tempC = c+freemanC[dirVec[i]];
	//	pos = tempR*w +tempC;
	//	if(ptrDataBinary[pos]){
	//		r=tempR;
	//		c=tempC;
	//		dir = dirVec[i];
	//		break;
	//	}
	//	ptrDataLabels[pos] = (int)currentBlob;
	//}
	//singlePixBlob = i == nDirs[d][p];
	//pos = r*w+c;

}

void myCompLabeler::getNextPointCW()
{
	//Dir:
	//321
	//4-0
	//567
	dir= (dir+2)%8;
	for(int i=0;i<8;i++){
		tempR = r+freemanR[dir];
		tempC = c+freemanC[dir];
		if(!(tempR<0 || tempR>=h || tempC<0 || tempC>=w)){
			pos = tempR*w +tempC;
			if(ptrDataBinary[pos]){
 				r=tempR;
 				c=tempC;
				break;
			}
			ptrDataLabels[pos] = (int)currentBlob;
		}
		dir--;
		dir = dir%8;	//To cycle through values 7->0
	}
	pos = r*w+c;
}



int myCompLabeler::freemanC[8] = {1,1,0,-1,-1,-1,0,1};

int myCompLabeler::freemanR[8] = {0,-1,-1,-1,0,1,1,1};

void myCompLabelerGroup::doLabeling(Blob_vector &blobs)
{	
	t_labelType label = 0;
	if(numThreads>1){
		//Preliminary step in order to pre-compute all the blobs crossing the border
		for(int i=1;i<numThreads;i++){
			Point offset(img.size().width,1);
			myCompLabeler lbl(img,labelers[i]->startPoint,labelers[i]->startPoint+offset,labels);
			lbl.Label();
			//cout << "Single pass\t" << lbl.blobs.size()<<endl;
			for(int i=0;i<lbl.blobs.size();i++){
				lbl.blobs[i]->SetID(label);
				label++;
				blobs.push_back(lbl.blobs[i]);
			}
		}
		for(int i=0;i<numThreads;i++){
			pthread_create(&tIds[i],NULL,(void*(__cdecl *)(void*))myCompLabeler::thread_Labeling,labelers[i]);
		}
		for(int i=0;i<numThreads;i++){
			pthread_join(tIds[i],0);
		}
	}
	else{
		labelers[0]->Label();
	}
	for(int i=0;i<numThreads;i++){
		//cout << "MT pass " <<i<<"\t" << labelers[i]->blobs.size()<<endl;
		for(int j=0;j<labelers[i]->blobs.size();j++){
			labelers[i]->blobs[j]->SetID(label);
			label++;
			blobs.push_back(labelers[i]->blobs[j]);
		}
	}
}

myCompLabelerGroup::myCompLabelerGroup()
{
	labelers=NULL;
	tIds=NULL;
}

myCompLabelerGroup::~myCompLabelerGroup()
{
	if(labelers){
		for(int i=0;i<numThreads;i++){
				delete labelers[i];
		}
		delete []labelers;
	}
	delete [] tIds;
}

void myCompLabelerGroup::set( int numThreads, Mat img )
{
	this->numThreads=numThreads;
	this->img=img;
	this->labels = Mat_<int>::zeros(img.size());
	if(tIds)
		delete []tIds;
	if(labelers){
		for(int i=0;i<numThreads;i++){
			delete labelers[i];
		}
		delete []labelers;
	}
	tIds = new pthread_t[numThreads];
	labelers = new myCompLabeler*[numThreads];
	Size sz = img.size();
	int numPx = sz.width*sz.width;
	int i=0;
	for(i;i<numThreads-1;i++){
		int yStart = (float)i/numThreads*sz.height;
		int yEnd = (float)(i+1)/numThreads*sz.height;
		Point st(0,yStart);
		Point en(sz.width,yEnd);
		labelers[i] = new myCompLabeler(img,st,en,labels);
	}
	Point st(0,(float)i/numThreads*sz.height);
	Point en((float)sz.width,sz.height);
	labelers[i] = new myCompLabeler(img,st,en,labels);
}

void myCompLabelerGroup::Reset()
{
	labels.setTo(0);
	for(int i=0;i<numThreads;i++)
		labelers[i]->Reset();
}

