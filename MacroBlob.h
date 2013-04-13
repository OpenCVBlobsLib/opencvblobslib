#pragma once
#include "blob.h"
#include "Segment.h"
#include <opencv2/opencv.hpp>
#include <pthread.h>
#include <vector>

/* Classe per gestire il join dei blob dopo la detection multithread
*/
class MacroBlob
{
public:
	vector<CBlob*> blobsToJoin;
	vector<Segment> commonSegments;
	CBlob *joinedBlob; //Obtained after the joining operation
public:
	MacroBlob(void);
	~MacroBlob(void);
	void join();
	bool contains(CBlob* blob);
private:
	Point chainCode2Point(Point org,t_chainCode code);
};

class MacroBlobJoiner{
private:
	vector<MacroBlob> &macroBlobs;
	pthread_t *threadIds;
	pthread_mutex_t mutex;
	int currentIndex;
	int numCores;
	int getIndexThread(); //Per garantire che i thread prendano macroblobs successivi
	static void* threadJoiner(MacroBlobJoiner *instance);
public:
	MacroBlobJoiner(vector<MacroBlob> &vect);
	~MacroBlobJoiner();
	void JoinAll();
};

