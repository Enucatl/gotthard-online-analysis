/*
 * FrameLoader.h
 *
 *  Created on: Oct 2, 2012
 *      Author: l_cartier
 */

#ifndef FRAMELOADER_H_
#define FRAMELOADER_H_

#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>

#define BUFFER_LENGTH 1286
#define HALFNCH 640
#define CHANNEL_NUMBER 1280

#define FRAMES_PER_FILE 20000

using namespace std;

#ifndef __CINT__
#define dout (*debugout)
#else
#define dout cout
#endif

//#define DEBUG

#define tmpDir "/tmp/GotthardFileSets_"

struct mypack {
	// char someheader[];
	unsigned int framenum;
	unsigned short inar[HALFNCH+1];
	//unsigned short space;
};

struct FullFrame {
	unsigned int frame1_number;
	unsigned int frame2_number;

	unsigned short inar[CHANNEL_NUMBER];
};

class FrameLoader {
public:
	FrameLoader(string* _filePrefix){

#ifndef DEBUG
		ofstream *dd = new ofstream();
		dd->open("/dev/null");
		debugout = dd;
#else
		debugout = &cout;
#endif

		analyzeFileSetName(_filePrefix);

		internalError = 0;
		firstFrameNumber = readFirstFrameNumber(0);
		if(internalError<0){
			cerr << "SmallFrameLoader: Can not open Dataset: " << (*filePrefix) << " (int err=" << internalError << ")" << endl;
			firstFrameNumber = 0;
			lastFrameNumber = 0;
			return;
		}
		if(!readStartIndexMap()){
			cout << "recreating index ..." << flush;
			createStartIndexMap();
			storeStartIndexMap();
			cout << "done" << endl;
		}else{
			storeStartIndexMap();
		}



		dout << "First Frame Number of files: " << *filePrefix << "* is: " << firstFrameNumber << endl;
		dout << "Last  Frame Number of files: " << *filePrefix << "* is: " << lastFrameNumber << endl;
		dout << "There are " << lastFrameNumber-firstFrameNumber << " frames (expect lost frames) in this file set" << endl;
		dout << "Offset is " << offset << endl;
		dout << "Total files: " << startIndexMap.size() << endl;
	}
	virtual ~FrameLoader(){
		dout << "deconstructor of FrameLoader called" << endl;
	}


	void update();

	vector<FullFrame*> readFrames(unsigned int startFrameNumber, unsigned int amountOfFullFrames);
	FullFrame *readFrame(unsigned int frameNumber);
	vector<FullFrame*> loadAll();

	string *filePrefix;
	unsigned int firstFrameNumber;
	unsigned int lastFrameNumber;
	unsigned int offset;
	map<int,unsigned int> startIndexMap;
	int isGuiFileSet;
private:
	void readNextCompleteFrame(FILE *f, FullFrame *ff, mypack *firstHalf = NULL);
	FILE *openFile(int number);
	unsigned int readFirstFrameNumber(int fileNumber);
	unsigned int readLastFrameNumber(int fileNumber);
	bool readHalfFrame(FILE *f,mypack *p);
	int createStartIndexMap(int fileNr = 0);
	void storeStartIndexMap();
	bool readStartIndexMap();
	FILE *findFileWithFrame(unsigned int frameNumber);
	FILE *_findFileWithFrame(unsigned int frameNumber);
	void readHalfFrameStartingFrom(FILE *f,mypack *p, unsigned int frameNumber);
	
	void analyzeFileSetName(string *_filePrefix);

	ostream *debugout;
	int internalError;

	int trialNumber;
};

#endif /* FRAMELOADER_H_ */
