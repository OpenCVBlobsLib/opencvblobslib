#include "MacroBlob.h"
#include "blob.h"
using namespace cv;

MacroBlob::MacroBlob(void)
{
	joinedBlob=NULL;
	toJoin=true;
}


MacroBlob::~MacroBlob(void)
{
}

void MacroBlob::join()
{
	if(!toJoin){
		joinedBlob=NULL;
		return;
	}
	//DEBUG: Per ora mostro i blobs ed i segmenti
//  	namedWindow("Temp",CV_WINDOW_NORMAL+CV_WINDOW_KEEPRATIO);
//  	Mat_<Vec3b> tempImg = Mat_<Vec3b>::zeros(blobsToJoin[0]->m_originalImageSize);
//  	RNG rng;
// 	CvSeqReader read;
// 	t_chainCode ch;
//  	for(int i=0;i<blobsToJoin.size();i++){
// 		Vec3b col(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));
// 		CvPoint po = blobsToJoin[i]->GetExternalContour()->GetStartPoint();
//  		Scalar color(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));
//  		cvStartReadSeq(blobsToJoin[i]->GetExternalContour()->m_contour,&read);
// 		for(int j=0;j<blobsToJoin[i]->GetExternalContour()->m_contour->total;j++){
// 			CV_READ_SEQ_ELEM(ch,read);
// 			po = chainCode2Point(po,ch);
// 			tempImg.at<Vec3b>(po) = col;
// 		}
//  	}
//  	for(int i=0;i<commonSegments.size();i++){
//  		Scalar color(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));
//  		commonSegments[i].DrawSegment(tempImg,color);
//  	}
//  	imshow("Temp",tempImg);
//  	waitKey();

	//Trovo le righe nelle quali si trovano i segmenti comuni
	vector<int> segmentRows;
	for(int i=0;i<commonSegments.size();i++){
		int j=0;
		for(j;j<segmentRows.size();j++){
			if(commonSegments[i].begin.y == segmentRows[j])
				break;
		}
		if (j==segmentRows.size())
			segmentRows.push_back(commonSegments[i].begin.y);
	}

	 
	//Per la scelta del primo blob prendo quello pi� in alto perch� altrimenti potrei avere blob con lo starting point
	//che si trova su una delle linee sovrapposte. Impongo anche il vincolo di non avere chaincodelist nulla
	CBlob* blob;
	Point p0 = Point(-1,blobsToJoin[0]->m_originalImageSize.height);
	int ind=-1;
	for(int i=0;i<blobsToJoin.size();i++){
		Point p1 = blobsToJoin[i]->GetExternalContour()->GetStartPoint();
		if(p1.y < p0.y && blobsToJoin[i]->GetExternalContour()->m_contour->total>0){
			ind = i;
			p0 = p1;
		}
	}
	//Tutti i blob considerati sono di 1 punto solo
	if(ind == -1){
		joinedBlob= NULL;
		return;
	}
	blob = blobsToJoin[ind];
	//Forse memory leak?
	joinedBlob = new CBlob(-1,blob->GetExternalContour()->GetStartPoint(),Size(-1,-1));
	t_chainCodeList chainList = blob->GetExternalContour()->GetChainCode();
	t_chainCodeList newChain = cvCreateSeq(chainList->flags,chainList->header_size,chainList->elem_size,joinedBlob->m_storage);
	t_chainCode chainCode;
	Point pt = blob->GetExternalContour()->GetStartPoint();
	Point originalStartingPt = blob->GetExternalContour()->GetStartPoint();
	CvSeqReader reader;
	CvSeqWriter writer;
	cvStartWriteSeq(chainList->flags,chainList->header_size,chainList->elem_size,joinedBlob->m_storage,&writer);
	cvStartReadSeq(chainList,&reader);

	
	int segmentIndex = -1;
	//Mat_<Vec3b> tempImg2= Mat_<Vec3b>::zeros(blobsToJoin[0]->m_originalImageSize);
	bool reached = false;
	do{
		//Vec3b color =Vec3b(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));
		//While che segue il contorno
		while(true){
			CV_READ_SEQ_ELEM(chainCode,reader);
			CV_WRITE_SEQ_ELEM(chainCode,writer);
			pt= chainCode2Point(pt,chainCode);
// 			tempImg2.at<Vec3b>(pt) = color;
// 			namedWindow("TempImg2",CV_WINDOW_NORMAL+CV_WINDOW_KEEPRATIO);
// 			imshow("TempImg2",tempImg2);
// 			int key = waitKey(1);
// 			if(key == ' ')
// 				while(waitKey(1)!=' ');
			if(pt == originalStartingPt){
				reached=true;
				break;
			}
			//Ho dovuto mettere questo controllo per evitare un errore causato dallo scorrere la sequenza in modo circolare
			else if(pt == Point(blob->GetExternalContour()->GetStartPoint())) 
				cvStartReadSeq(blob->GetExternalContour()->m_contour,&reader);
			int j=0;
			for(j;j<segmentRows.size();j++)
				if(pt.y == segmentRows[j])
					break;
			if(j<segmentRows.size()){
				int i=0;
				for(i;i<commonSegments.size();i++)
					if(commonSegments[i].isExtremum(pt)){
						segmentIndex = i;
						break;
					}
				if(i<commonSegments.size())
					break;
			}
		}
		if(reached)
			break;
		//waitKey();
		blob = Point(blob->GetExternalContour()->GetStartPoint()) == Point(commonSegments[segmentIndex].blobA->GetExternalContour()->GetStartPoint()) ? commonSegments[segmentIndex].blobB : commonSegments[segmentIndex].blobA;
		if(blob->GetExternalContour()->m_contour->total > 0){
			//Scorro il contour del blob successivo fino a raggiungere il punto in cui mi sono fermato precedentemente.
			cvStartReadSeq(blob->GetExternalContour()->GetChainCode(),&reader);
			Point tempPoint = pt;
			pt = blob->GetExternalContour()->GetStartPoint();
			while(pt!=tempPoint){
				CV_READ_SEQ_ELEM(chainCode,reader);
				pt = chainCode2Point(pt,chainCode);
			}
		}
		else{
			//Se il blob non ha chain code (� formato probabilmente da un punto solo) proseguo con il blob corrente.
			blob = Point(blob->GetExternalContour()->GetStartPoint()) == Point(commonSegments[segmentIndex].blobA->GetExternalContour()->GetStartPoint()) ? commonSegments[segmentIndex].blobB : commonSegments[segmentIndex].blobA;
		}
	} while(pt!=originalStartingPt);
	newChain = cvEndWriteSeq(&writer);
	cvClearSeq(joinedBlob->GetExternalContour()->m_contour);
	joinedBlob->GetExternalContour()->m_contour = newChain;
	for(int i=0;i<blobsToJoin.size();i++)
		blobsToJoin[i]->to_be_deleted=1;
	

	//Images to show only for debug purposes
// 	Mat_<Vec3b> temp = Mat_<Vec3b>::zeros(1200,1200);
// 	cvStartReadSeq(newChain,&reader);
// 	bool disp=true;
// 	int key;
// 	namedWindow("Prova",CV_WINDOW_NORMAL+CV_WINDOW_KEEPRATIO);
// 	CvPoint point = joinedBlob->GetExternalContour()->GetStartPoint();
// 	temp.at<Vec3b>(point) = Vec3b(0,255,0);
// 	int i=0;
// 	for(i=0;i<newChain->total;i++){
// 		CV_READ_SEQ_ELEM(chainCode,reader);
// 		point = chainCode2Point(point,chainCode);
// 		if(i < newChain->total/2)
// 			temp.at<Vec3b>(point) = Vec3b(0,255,0);
// 		else
// 			temp.at<Vec3b>(point) = Vec3b(0,255,255);
// 		if(disp){
// 			imshow("Prova",temp);
// 			key = waitKey(1);
// 			if(key == ' ')
// 				disp=false;
// 		}
// 	}
// 	imshow("Prova",temp);
// 	waitKey();
}

bool MacroBlob::contains( CBlob* blob )
{
	int sz = blobsToJoin.size();
	for(int i=0;i< sz;i++)
		if(blobsToJoin[i]->GetExternalContour()->GetStartPoint().x == blob->GetExternalContour()->GetStartPoint().x && blobsToJoin[i]->GetExternalContour()->GetStartPoint().y == blob->GetExternalContour()->GetStartPoint().y)
			return true;
	return false;
}

Point MacroBlob::chainCode2Point( Point org,t_chainCode code )
{
	//	/* Luca Nardelli & Saverio Murgia
	//	Freeman Chain Code:	
	//		321		Values indicate the chain code used to identify next pixel location.
	//		4-0		If I join 2 blobs I can't just append the 2nd blob chain codes, since they will still start
	//		567		from the 1st blob start point
	//	*/
	Point increments[8];
	increments[0] = Point(1,0);
	increments[1] = Point(1,-1);
	increments[2] = Point(0,-1);
	increments[3] = Point(-1,-1);
	increments[4] = Point(-1,0);
	increments[5] = Point(-1,1);
	increments[6] = Point(0,1);
	increments[7] = Point(1,1);
	return org +increments[code];
}

MacroBlobJoiner::MacroBlobJoiner(vector<MacroBlob> &vect, int numT):macroBlobs(vect),numThreads(numT)
{
	threadIds = new pthread_t[numThreads];
	mutex = PTHREAD_MUTEX_INITIALIZER;
	currentIndex=0;
}

MacroBlobJoiner::~MacroBlobJoiner()
{
	delete [] threadIds;
}

void MacroBlobJoiner::JoinAll()
{
	//Per ragioni di debug
	for(int i=0;i<macroBlobs.size();i++)
		macroBlobs[i].join();

// 	for(int i=0;i<numCores;i++){
// 		pthread_create(&threadIds[i],NULL,(void *(*)(void*))MacroBlobJoiner::threadJoiner,this);
// 	}
// 	for(int i=0;i<numCores;i++){
// 		pthread_join(threadIds[i],0);
// 	}
}

int MacroBlobJoiner::getIndexThread()
{
	if(currentIndex == macroBlobs.size())
		return -1;
	pthread_mutex_lock(&mutex);
	int ind = currentIndex++;
	pthread_mutex_unlock(&mutex);
	return ind;
}

void* MacroBlobJoiner::threadJoiner( MacroBlobJoiner *instance )
{
	int i = instance->getIndexThread();
	while(i!=-1){
		instance->macroBlobs[i].join();
		i = instance->getIndexThread();
	}
	return 0;
}
