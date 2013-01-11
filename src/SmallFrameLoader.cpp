/*
 * SmallFrameLoader.cpp
 *
 *  Created on: Oct 26, 2012
 *      Author: l_cartier
 */

#include "SmallFrameLoader.h"
#include <errno.h>
#include <stdio.h>

unsigned int SmallFrameLoader::readFirstFrameNumber(int fileNumber){
	FILE *firstFile = openFile(fileNumber);
	unsigned int firstIndex = -1;
	if(firstFile == NULL){
		internalError = -1;
		return -1;
	}
	SmallFrame *sf = (SmallFrame *)malloc(sizeof(SmallFrame));
	if(readSmallFrame(firstFile,sf)){
		firstIndex = sf->framenum;
	}else{
		internalError = -2;
		firstIndex = -2;
	}
	free(sf);
	fclose(firstFile);
	return firstIndex;
}

bool SmallFrameLoader::readSmallFrame(FILE *f,SmallFrame *sf){
	//cout << "try to read half frame" << endl;
	return fread(sf, BUFFER_LENGTH_SMALL, 1 ,f);
}

FILE *SmallFrameLoader::openFile(int number){
	char filename[256];
	sprintf(filename, "%s%09d.dat", filePrefix->c_str(), number);
	dout << "Try to open file " << filename << endl;
	FILE *f = fopen((const char *) (filename), "r");
	if(f == NULL){
		dout << "*.dat doesnt work" << endl;
		filename[0] = '\0';
		sprintf(filename, "%s%09d.raw", filePrefix->c_str(), number);
		dout << "Try to open file " << filename << endl;
		f = fopen((const char *) (filename), "r");
		if(f == NULL && ENOENT != errno){		
			dout << "*.raw doesnt work" << endl;
			perror("error while opening file: ");
		}
	}
	return f;
}

int SmallFrameLoader::createStartIndexMap(int fileNr){
	internalError = 0;
	unsigned int startFrameNumber = readFirstFrameNumber(fileNr);
	while(internalError>=0){
		startIndexMap[fileNr] = startFrameNumber;
		fileNr++;
		internalError = 0;
		startFrameNumber = readFirstFrameNumber(fileNr);

		if(fileNr % 100 == 0){
			dout << "SmallFrameLoader::createStartIndexMap: index for file Nr. " << fileNr << " startFrameNumber is : " << startFrameNumber << endl;
		}
	}
	lastFrameNumber = readLastFrameNumber(fileNr-1);
	dout << "SmallFrameLoader::createStartIndexMap: done" << endl;
	return fileNr;
}

unsigned int SmallFrameLoader::readLastFrameNumber(int fileNumber){
	dout << "SmallFrameLoader::readLastFrameNumber: find last frame in file: " << fileNumber << endl;
	unsigned int lfn;
	FILE *lf = openFile(fileNumber);
	if(lf != NULL){
	  fseek(lf,0,SEEK_END);
	  int fs = ftell(lf);
	  dout << "SmallFrameLoader::readLastFrameNumber: last file has length " << fs << endl;
	  dout << "SmallFrameLoader::readLastFrameNumber: file length % 518 = " << fs % 518 << endl;

	  fseek(lf,-518-(fs % 518),SEEK_END);
	  SmallFrame lastFrame;
	  if(readSmallFrame(lf,&lastFrame)<1){
	    cerr << "could not read last frame in last file" << endl;
	    lfn = -2;
	    internalError = -2;
	  }else{
	    lfn = lastFrame.framenum;
	  }
	}else{
	  cerr << "could not determine last framenumber of fileSet" << endl;
	  lfn = -1;
	  internalError = -1;
	}
	dout << "SmallFrameLoader::readLastFrameNumber: done" << endl;
	return lfn;
}

bool SmallFrameLoader::readStartIndexMap(){
	ifstream mapFile;
	stringstream mapFileName;
	mapFileName << string(tmpDir) << firstFrameNumber << string("_index.txt");
	dout << "SmallFrameLoader::readStartIndexMap(): Try to open File: " << mapFileName.str() << endl;
	mapFile.open(mapFileName.str().c_str());
	if(mapFile.is_open()){
		string dump, fileset;
		std::getline(mapFile,dump,':'); mapFile >> fileset;
		if(fileset == (*filePrefix)){
			startIndexMap.clear();
			int fileNum; unsigned int frameNum;
			while(mapFile >> fileNum >> frameNum){
				startIndexMap[fileNum] = frameNum;

			}
			dout << "SmallFrameLoader::readStartIndexMap(): read " << fileNum+1 << " entries. startIndex of last: " << frameNum << endl;
			dout << "SmallFrameLoader::readStartIndexMap(): check for newly added files" << endl;
			fileNum = createStartIndexMap(fileNum+1);
			dout << "SmallFrameLoader::readStartIndexMap(): now there are " << fileNum << " files indexed" << endl;
			return true;
		}else{
			dout << "SmallFrameLoader::readStartIndexMap(): File does exist BUT is for a different filePrefix -> drop it" << endl;
			return false;
		}
	}else{
		dout << "SmallFrameLoader::readStartIndexMap(): File does not exist yet  ...." << endl;
		return false;
	}
}

void SmallFrameLoader::storeStartIndexMap(){
	ofstream mapFile;
	stringstream mapFileName;
	mapFileName << string(tmpDir) << firstFrameNumber << string("_index.txt");
	dout << "SmallFrameLoader::storeStartIndexMap(): Writing File: " << mapFileName.str() << endl;
	mapFile.open(mapFileName.str().c_str());
	if(mapFile.is_open()){
		mapFile << "FILESET:" << (*filePrefix) << endl;
		for(map<int,unsigned int>::iterator it = startIndexMap.begin(); it != startIndexMap.end(); it++){
			mapFile << it->first << "\t" << it->second << endl;
		}
		mapFile.close();

		string *ARCmd = new string("chmod 666 ");
		(*ARCmd) += mapFileName.str();
		system(ARCmd->c_str());
		delete ARCmd;
	}else{
		dout << "SmallFrameLoader::storeStartIndexMap(): Opening failed  ...." << endl;
	}
}

bool SmallFrameLoader::readSmallFrameStartingFrom(FILE *f,SmallFrame *p,unsigned int frameNumber){
	while(readSmallFrame(f,p) && p->framenum < frameNumber);
	return(p->framenum >= frameNumber);
}


SmallFrame* SmallFrameLoader::readFrame(unsigned int startFrameNumber){
	FILE *f = findFileWithFrame(startFrameNumber);
	SmallFrame* firstff = (SmallFrame *)malloc(sizeof(SmallFrame));
	readSmallFrameStartingFrom(f,firstff,startFrameNumber);
	fclose(f);
	return firstff;
}

vector<SmallFrame*> SmallFrameLoader::readFrames(unsigned int startFrameNumber,unsigned int amountOfSmallFrames){
	dout << "SmallFrameLoader::readFrames: Start reading from: " << startFrameNumber << " will read " << amountOfSmallFrames << " frames" << endl;
	vector<SmallFrame *> v;
	SmallFrame *ff, *firstff;
	unsigned int lastFrameNumber;
	if(amountOfSmallFrames == 0){
		return v;
	}
	FILE *f = findFileWithFrame(startFrameNumber);
	if(f!=NULL){
		firstff = (SmallFrame *)malloc(sizeof(SmallFrame));
		readSmallFrameStartingFrom(f,firstff,startFrameNumber);

		v.push_back(firstff);
		lastFrameNumber = firstff->framenum;
		while(lastFrameNumber < startFrameNumber + amountOfSmallFrames){
			ff = (SmallFrame *)malloc(sizeof(SmallFrame));

			if(!readSmallFrame(f,ff)){
				//open next file and try again to read frame
				fclose(f);
				unsigned int nextFrameNumber = lastFrameNumber + (unsigned int)1;
				dout << "SmallFrameLoader::readFrames: EOF reached. Try to find file with frame number: " << nextFrameNumber << endl;
				f = findFileWithFrame(nextFrameNumber);
				if(f == NULL){
					cerr << "SmallFrameLoader::readFrames: reached end of dataset while reading frames" << endl;
					return v;
				}
				while(!readSmallFrameStartingFrom(f,ff,nextFrameNumber)){
					nextFrameNumber++;
					dout << "SmallFrameLoader::readFrames: This file does not contain requested frame!!! Try next frame: " << nextFrameNumber << endl;
					f = findFileWithFrame(nextFrameNumber);
				}
				dout << "SmallFrameLoader::readFrames: Ok. First frame of new file is: " << ff->framenum << endl;
			}
			v.push_back(ff);
			lastFrameNumber = ff->framenum;
			//dout << "SmallFrameLoader::readFrames: Read frame with framenumber: " << ff->framenum << endl;
		}
		fclose(f);
	}else{
		cerr << "SmallFrameLoader::readFrames: could not find frame with starting number: " << startFrameNumber << endl;
	}
	return v;
}

void SmallFrameLoader::update(){
    dout << "SmallFrameLoader::update: Okay ... will look for new data" << endl;
    readStartIndexMap();
	storeStartIndexMap();
}

FILE *SmallFrameLoader::findFileWithFrame(unsigned int frameNumber){
	unsigned int lowerBoundary = 0; int fileNumber;
	fileNumber = -1;

	if(frameNumber > lastFrameNumber){
		dout << "SmallFrameLoader::findFileWithFrame: End of dataset reached" << endl;
		dout << "SmallFrameLoader::findFileWithFrame: Will look for new data" << endl;
		readStartIndexMap();
		storeStartIndexMap();
		if(frameNumber > lastFrameNumber){
			cerr << "SmallFrameLoader::findFileWithFrame: Reached end of dataset and now new data available ..." << endl;
			abort();
			return NULL;
		}
	}

	for(map<int,unsigned int>::iterator it = startIndexMap.begin(); it != startIndexMap.end(); it++){
		unsigned int fileStartIndex = it->second;

		if(lowerBoundary < fileStartIndex && fileStartIndex <= frameNumber){
			lowerBoundary = fileStartIndex;
			fileNumber = it->first;
		}
	}
	if(fileNumber>=0){
		dout << "SmallFrameLoader::findFileWithFrame: File " << fileNumber << " should contain frame " << frameNumber << endl;
	}else{
		cerr << "SmallFrameLoader::findFileWithFrame: frame does not exist. Wrong framenumber or offset??" << endl;
		abort();
	}

	FILE* f = openFile(fileNumber);
	if(f == NULL){
		cerr << "SmallFrameLoader::findFileWithFrame: frame does not exist." << endl;
		abort();
	}
	return f;
}
