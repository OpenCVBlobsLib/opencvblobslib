#include "ComponentLabeling.h"

using namespace cv;

myCompLabeler::myCompLabeler(Mat &binImage,CBlobContour** lab,Point start,Point end):
	startPoint(start),endPoint(end),
	binaryImage(binImage)
{
	parent=NULL;
	labels = lab;
	r=0;c=0;
	dir=0;
}

myCompLabeler::~myCompLabeler()
{

}

void myCompLabeler::Reset()
{
	blobs.clear();
}

void myCompLabeler::Label()
{
	ptrDataBinary = binaryImage.data;
	ptrDataLabels = labels;
	currentBlob=NULL;

	h = binaryImage.size().height;
	w = binaryImage.size().width;
	CBlobContour* label=NULL;
	for(r=startPoint.y;r<endPoint.y;r++){
		//First col
		pos = r*w;
		c=startPoint.x;
		if(ptrDataBinary[pos]){
			label = ptrDataLabels[pos];
			if(label){
				currentBlob = label->parent;
			}
			//Else if so to not check for label==NULL
			else if(ptrDataBinary[pos] /*&& ptrDataLabels[pos]==NULL*/){
				currentBlob = new CBlob(currentLabel,Point(c,r),Size(w,h));
				blobs.push_back(currentBlob);
				TracerExt();
			}
			if(!ptrDataBinary[pos+1] && !ptrDataLabels[pos+1]){
				TracerInt();
			}
		}
		//Other cols
		for(c=startPoint.x+1;c<endPoint.x-1;c++){
			pos = r*w+c;
			if(ptrDataBinary[pos]){
				label = ptrDataLabels[pos];
				if(label!=0)
					currentBlob = label->parent;
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
				currentBlob = label->parent;
			else if(!ptrDataBinary[pos-1]){
				currentBlob = new CBlob(currentLabel,Point(c,r),Size(w,h));
				blobs.push_back(currentBlob);
				TracerExt();
			}
		}
	}
}

void* myCompLabeler::thread_Labeling( void* o )
{
	myCompLabeler *obj = (myCompLabeler*)o;
	obj->Label();
	return 0;
}

void myCompLabeler::TracerExt()
{
	//Dir:
	//321
	//4-0
	//567
	//cout << "TracerExt\t" << c<<","<<r<<endl;
	currentContour=&currentBlob->m_externalContour;
#ifdef DEBUG_COMPONENT_LABELLING
 	cout << "TracerExt\t" << c<<","<<r<<endl;
 	bool debugDraw=true;
#endif
	int sR=r,sC=c;
	int startPos = sR*w+sC;
	dir=6;
#ifdef DEBUG_COMPONENT_LABELLING
	ptrDataBinary[pos]= 150; //Debug
#endif
  	getNextPointCCW();
 	if(singlePixBlob){
 		r=sR;
 		c=sC;
 		pos = r*w+c;
		ptrDataLabels[pos] = currentContour;
 		return;
 	}
	t_chainCodeList *cont = &currentBlob->m_externalContour.m_contour[0];
	while(pos!=startPos){
#ifdef DEBUG_COMPONENT_LABELLING
		ptrDataBinary[pos]= 150;
#endif
		cont->push_back(dir);
		ptrDataLabels[pos] = currentContour;
		getNextPointCCW();
#ifdef DEBUG_COMPONENT_LABELLING
		if(debugDraw){
    		namedWindow("im",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
    		imshow("im",binaryImage);
 		stringstream ss;
 		ss << c << "," << r;
 		displayOverlay("im",ss.str());
    		int k = waitKey();
    		if(k==' ')
    			debugDraw=false;
    	}	
#endif
	}
	cont->push_back(dir);
	ptrDataLabels[pos] = currentContour;
	//For blobs in which the starting point must be crossed many times
	for(int i=0;i<3;i++){
		getNextPointCCW();
		if(ptrDataLabels[pos]==0){
#ifdef DEBUG_COMPONENT_LABELLING
			ptrDataBinary[pos]=150;
#endif
			while(pos!=startPos){
				//cout << r << "," << c << endl;
				ptrDataLabels[pos] = currentContour;
				cont->push_back(dir);
				getNextPointCCW();
#ifdef DEBUG_COMPONENT_LABELLING
				ptrDataBinary[pos]= 150;
				if(debugDraw){
				 	namedWindow("im",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
				 	imshow("im",binaryImage);
				 	int k = waitKey();
				 	if(k==' ')
				 		debugDraw=false;
				}
#endif
			}
			cont->push_back(dir);
			ptrDataLabels[pos] = currentContour;
		}
		else{
			r=sR;
			c=sC;
			pos = r*w+c;
			break;
		}
	}
}

void myCompLabeler::TracerInt( int startDir /*= 5*/ )
{
	//Dir:
	//321
	//4-0
	//567
	//cout << "TracerInt\t" << c<<","<<r<<endl;
#ifdef DEBUG_COMPONENT_LABELLING
	cout << "TracerInt\t" << c<<","<<r<<endl;
	bool debugDraw=true;
#endif
	int sR=r,sC=c;
	int startPos = sR*w+sC;
	currentContour = new CBlobContour(Point(c,r),Size(w,h));
	currentContour->parent=currentBlob;
	t_chainCodeList *cont = &currentContour->m_contour[0];
	dir=startDir;
#ifdef DEBUG_COMPONENT_LABELLING
	ptrDataBinary[pos] = 50;
#endif
	getNextPointCW();
	uchar firstDir = dir;
#ifdef DEBUG_COMPONENT_LABELLING
	ptrDataBinary[pos] = 100;
#endif
	cont->push_back(dir);
	ptrDataLabels[pos] = currentContour;
	while(pos!=startPos){
		// 		cout << r << "," << c << endl;
		getNextPointCW();
		ptrDataLabels[pos] = currentContour;
		cont->push_back(dir);
#ifdef DEBUG_COMPONENT_LABELLING
		ptrDataBinary[pos] = 100;
  		if(debugDraw){
  		 	namedWindow("im",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
  		 	imshow("im",binaryImage);
 			stringstream ss;
 			ss << c << "," << r;
 			displayOverlay("im",ss.str());
  		 	int k = waitKey();
  		 	if(k==' ')
  		 		debugDraw=false;
  		}
#endif
	}

	//For internal contours in which the starting point must be crossed many times, like:
	//	ooooooooooooooo
	//	ooooos        o
 	//	ooooo o       o
	//	ooooo  o      o
	//	ooooo         o
	//	ooooooooooooooo
	for(int i=0;i<3;i++){
		getNextPointCW();
		//In tracerExt i check for label==0, beacuse there can not be situations in which a pixel belongs to 2 contours.
		//This can happen with internal contorus, so I have to modify the condition.
		if(ptrDataLabels[pos]!=currentContour){ 
#ifdef DEBUG_COMPONENT_LABELLING
			ptrDataBinary[pos]=100;
#endif
			while(pos!=startPos){
				ptrDataLabels[pos] = currentContour;
				cont->push_back(dir);
				getNextPointCW();
#ifdef DEBUG_COMPONENT_LABELLING
				ptrDataBinary[pos]= 100;
				if(debugDraw){
					namedWindow("im",CV_WINDOW_NORMAL + CV_GUI_EXPANDED + CV_WINDOW_KEEPRATIO);
					imshow("im",binaryImage);
					int k = waitKey();
					if(k==' ')
						debugDraw=false;
				}
#endif
			}
			cont->push_back(dir);
			ptrDataLabels[pos] = currentContour;
		}
		else{
			r=sR;
			c=sC;
			pos = r*w+c;
			break;
		}
	}


	//If labeler has parent it means that I'm using more than 1 thread.
	//Then I must check for collisions when adding internal contours to the same blob.
	if(parent){
		parent->acquireMutex();
		currentBlob->m_internalContours.push_back(currentContour);
		parent->releaseMutex();
	}
	else
		currentBlob->m_internalContours.push_back(currentContour);
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
			ptrDataLabels[pos] = currentContour;
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
			ptrDataLabels[pos] = currentContour;
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
			myCompLabeler lbl(img,labels,labelers[i]->startPoint,labelers[i]->startPoint+offset);
			lbl.parent=this;
			lbl.Label();
			//cout << "Single pass\t" << lbl.blobs.size()<<endl;
			for(unsigned int i=0;i<lbl.blobs.size();i++){
				lbl.blobs[i]->SetID(label);
				label++;
				blobs.push_back(lbl.blobs[i]);
			}
		}
		for(int i=0;i<numThreads;i++){
			pthread_create(&tIds[i],NULL,myCompLabeler::thread_Labeling,labelers[i]);
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
		for(unsigned int j=0;j<labelers[i]->blobs.size();j++){
			labelers[i]->blobs[j]->SetID(label);
			label++;
			blobs.push_back(labelers[i]->blobs[j]);
		}
	}
}

myCompLabelerGroup::myCompLabelerGroup()
{
	mutexBlob = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	labels=NULL;
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
	if(tIds)
		delete [] tIds;
	if(labels)
		delete [] labels;
}

void myCompLabelerGroup::set( int nThreads, Mat binIm )
{
	this->numThreads=nThreads;
	if(binIm.isContinuous()){
		this->img=binIm;
	}
	else{
		this->img=binIm.clone();
	}
	//this->labels = Mat_<int>::zeros(img.size());
	if (labels){
		delete [] labels;
	}
	int nPts = binIm.size().width*binIm.size().height;
	labels = new CBlobContour*[nPts];
	memset(labels,0,nPts*sizeof(CBlob*));

	if(tIds)
		delete []tIds;
	if(labelers){
		for(int i=0;i<nThreads;i++){
			delete labelers[i];
		}
		delete []labelers;
	}
	tIds = new pthread_t[nThreads];
	labelers = new myCompLabeler*[nThreads];
	Size sz = binIm.size();
	int numPx = sz.width*sz.width;
	int i=0;
	for(i;i<nThreads-1;i++){
		int yStart = (int)((float)i/nThreads*sz.height);
		int yEnd = (int)((float)(i+1)/nThreads*sz.height);
		Point st(0,yStart);
		Point en(sz.width,yEnd);
		labelers[i] = new myCompLabeler(img,labels,st,en);
		labelers[i]->parent=this;
	}
	Point st(0,(int)((float)i/nThreads*sz.height));
	Point en(sz.width,sz.height);
	//st = Point(0,187);
	labelers[i] = new myCompLabeler(img,labels,st,en);
	labelers[i]->parent=this;
}

void myCompLabelerGroup::Reset()
{
	if(labels){
		delete []labels;
		labels=NULL;
	}
	for(int i=0;i<numThreads;i++)
		labelers[i]->Reset();
}

void myCompLabelerGroup::acquireMutex()
{
	pthread_mutex_lock(&mutexBlob);
}

void myCompLabelerGroup::releaseMutex()
{
	pthread_mutex_unlock(&mutexBlob);
}

