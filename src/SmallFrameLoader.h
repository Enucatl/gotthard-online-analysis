/*
 * SmallFrameLoader.h
 *
 *  Created on: Oct 26, 2012
 *      Author: l_cartier
 */

#ifndef SMALLFRAMELOADER_H_
#define SMALLFRAMELOADER_H_

#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>

#define BUFFER_LENGTH_SMALL 518
#define CHANNEL_NUMBER_SMALL 256

using namespace std;

#ifndef __CINT__
#define dout (*debugout)
#else
#define dout cout
#endif

//#define DEBUG

#define tmpDir "/tmp/GotthardFileSets_"

struct SmallFrame {
	// char someheader[];
	unsigned int framenum;
	unsigned short inar[CHANNEL_NUMBER_SMALL+1];
	//unsigned short space;
};


class SmallFrameLoader {
public:
	SmallFrameLoader(string* _filePrefix){

#ifndef DEBUG
		ofstream *dd = new ofstream();
		dd->open("/dev/null");
		debugout = dd;
#else
		debugout = &cout;
#endif

		filePrefix = _filePrefix;

		istringstream *filePrefixStream = new istringstream(_filePrefix->c_str());
		filePrefix = new string();
		long long int dataOffset = 0;

		std::getline(*filePrefixStream,*filePrefix,':');
		(*filePrefixStream) >> dataOffset;
		dout << "dataOffset is " << dataOffset << endl;
		dout << "sizeof(dataOffset) is " << sizeof(dataOffset) << endl;

		internalError = 0;
		firstFrameNumber = readFirstFrameNumber(0);
		if(internalError<0){
			cerr << "SmallFrameLoader: Can not open Dataset: " << (*filePrefix) << endl;
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

		if(dataOffset < 0){
			offset = lastFrameNumber - (unsigned int)(-1l * dataOffset) - firstFrameNumber;
		}else{
			offset = (unsigned int)dataOffset;
		}

		dout << "First Frame Number of files: " << *filePrefix << "* is: " << firstFrameNumber << endl;
		dout << "Last  Frame Number of files: " << *filePrefix << "* is: " << lastFrameNumber << endl;
		dout << "There are " << lastFrameNumber-firstFrameNumber << " frames (expect lost frames) in this file set" << endl;
		dout << "Offset is " << offset << endl;
		dout << "Total files: " << startIndexMap.size() << endl;
	}
	virtual ~SmallFrameLoader(){
		dout << "deconstructor of FrameLoader called" << endl;
	}

	vector<SmallFrame*> readFrames(unsigned int startFrameNumber, unsigned int amountOfFullFrames);
	SmallFrame* readFrame(unsigned int startFrameNumber);
    void update();

	string *filePrefix;
	unsigned int firstFrameNumber;
	unsigned int lastFrameNumber;
	unsigned int offset;
	map<int,unsigned int> startIndexMap;


private:
	bool readSmallFrame(FILE *f,SmallFrame *sf);
	FILE *openFile(int number);
	unsigned int readFirstFrameNumber(int fileNumber);
	unsigned int readLastFrameNumber(int fileNumber);
	int createStartIndexMap(int fileNr = 0);
	void storeStartIndexMap();
	bool readStartIndexMap();
	FILE *findFileWithFrame(unsigned int frameNumber);
	bool readSmallFrameStartingFrom(FILE *f,SmallFrame *p,unsigned int frameNumber);

	ostream *debugout;
	int internalError;
};

#endif /* SMALLFRAMELOADER_H_ */
