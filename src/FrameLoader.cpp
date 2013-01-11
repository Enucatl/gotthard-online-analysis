/*
 * FrameLoader.cpp
 *
 *  Created on: Oct 2, 2012
 *      Author: l_cartier
 */

#include "FrameLoader.h"
#include <stdio.h>
#include <errno.h>
#include <cstring>


void FrameLoader::analyzeFileSetName(string *_filePrefix){
	long long int dataOffset = 0;
	trialNumber = -1;
	filePrefix = new string();
	istringstream *filePrefixStream = new istringstream(_filePrefix->c_str());
	string withoutTrialNumber;
	(*filePrefixStream) >> withoutTrialNumber;
	(*filePrefixStream) >> trialNumber;

	dout << "FrameLoader::analyzeFileSetName: without trial number " << withoutTrialNumber << endl;
	dout << "FrameLoader::analyzeFileSetName: trial number is " << trialNumber << endl;

	istringstream *withoutTrialNumberStream = new istringstream(withoutTrialNumber.c_str());

	std::getline(*withoutTrialNumberStream,*filePrefix,':');
	(*withoutTrialNumberStream) >> dataOffset;


	dout << "FrameLoader::analyzeFileSetName: dataOffset is " << dataOffset << endl;
	dout << "FrameLoader::analyzeFileSetName: filePrefix " << (*filePrefix) << endl;


	if(dataOffset < 0){
		offset = lastFrameNumber - (unsigned int)(-1l * dataOffset) - firstFrameNumber;
	}else{
		offset = (unsigned int)dataOffset;
	}

	if(trialNumber >= 0){
		dout << "FrameLoader::analyzeFileSetName: enable isGuiFileSet mode" << endl;
		isGuiFileSet = true;
	}else{
		isGuiFileSet = false;
	}

}

unsigned int FrameLoader::readFirstFrameNumber(int fileNumber){
	FILE *firstFile = openFile(fileNumber);
	unsigned int firstIndex = -1;
	if(firstFile == NULL){
		internalError = -1;
		return -1;
	}
	mypack *p = (mypack *)malloc(sizeof(mypack));
	if(readHalfFrame(firstFile,p)){
		firstIndex = p->framenum;
	}else{
		internalError = -2;
		firstIndex = -2;
	}
	free(p);
	fclose(firstFile);
	return firstIndex;
}

bool FrameLoader::readHalfFrame(FILE *f,mypack *p){
	//cout << "try to read half frame" << endl;
	return fread(p, BUFFER_LENGTH, 1 ,f);
}

FILE *FrameLoader::openFile(int number){
	char filename[256];
	FILE *f;
	if(isGuiFileSet){
		sprintf(filename, "%sf%012d_%i.raw", filePrefix->c_str(), number*FRAMES_PER_FILE,trialNumber);
		dout << "FrameLoader::openFile: Try to open file " << filename << endl;
		f = fopen((const char *) (filename), "r");
		if(f == NULL){
			dout << "FrameLoader::openFile: can not open " << filename << endl;
		}
	}else{
		sprintf(filename, "%s%09d.dat", filePrefix->c_str(), number);
		dout << "FrameLoader::openFile: Try to open file " << filename << endl;
		f = fopen((const char *) (filename), "r");
		if(f == NULL){
			dout << "FrameLoader::openFile: *.dat doesnt work" << endl;
			filename[0] = '\0';
			sprintf(filename, "%s%09d.raw", filePrefix->c_str(), number);
			dout << "FrameLoader::openFile: Try to open file " << filename << endl;
			f = fopen((const char *) (filename), "r");
			if(f == NULL && ENOENT != errno){
				dout << "FrameLoader::openFile: *.raw doesnt work" << endl;
				perror("error while opening file: ");
			}
		}
	}
	return f;
}


int FrameLoader::createStartIndexMap(int fileNr){
	internalError = 0;
	unsigned int startFrameNumber = readFirstFrameNumber(fileNr);
	while(internalError>=0){
		startIndexMap[fileNr] = startFrameNumber;
		fileNr++;
		internalError = 0;
		startFrameNumber = readFirstFrameNumber(fileNr);

		if(fileNr % 100 == 0){
			dout << "FrameLoader::createStartIndexMap: index for file Nr. " << fileNr << " startFrameNumber is : " << startFrameNumber << endl;
		}
	}
	lastFrameNumber = readLastFrameNumber(fileNr-1);
	dout << "FrameLoader::createStartIndexMap: done" << endl;
	return fileNr;
}

unsigned int FrameLoader::readLastFrameNumber(int fileNumber){
  dout << "FrameLoader::readLastFrameNumber: find last frame in file: " << fileNumber << endl;
  unsigned int lfn = 0;
  FILE *lf = openFile(fileNumber);
  if(lf != NULL){
    fseek(lf,0,SEEK_END);
    int fs = ftell(lf);
    dout << "FrameLoader::readLastFrameNumber: last file has length " << fs << endl;
    dout << "FrameLoader::readLastFrameNumber: file length % 1286 = " << fs % 1286 << endl;
    dout << "FrameLoader::readLastFrameNumber: will search at position " << fs -1286-(fs%1286) << " for last frame number" << endl;
    fseek(lf,-1286-(fs%1286),SEEK_END);
    mypack lastFrame;
    if(readHalfFrame(lf,&lastFrame)<1){
      cerr << "could not read last frame in last file" << endl;
      lfn = -2;
      internalError = -2;
    }else{
      lfn = lastFrame.framenum;
      //	printf("%i: last frame hex: 0x%x  %u\n",i,lfn,lfn);
      
    }
    //}
  }else{
    cerr << "could not determine last framenumber of fileSet" << endl;
    lfn = -1;
    internalError = -1;
  }
  dout << "FrameLoader::readLastFrameNumber: done (" << lfn << ")" << endl;
  return lfn;
}

bool FrameLoader::readStartIndexMap(){
	ifstream mapFile;
	stringstream mapFileName;
	mapFileName << string(tmpDir) << firstFrameNumber << string("_indexBigFrame.txt");
	dout << "FrameLoader::readStartIndexMap(): Try to open File: " << mapFileName.str() << endl;
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
			dout << "FrameLoader::readStartIndexMap(): read " << fileNum << " entries. startIndex of last: " << frameNum << endl;
			dout << "FrameLoader::readStartIndexMap(): check for newly added files" << endl;
			fileNum = createStartIndexMap(fileNum+1);
			dout << "FrameLoader::readStartIndexMap(): now there are " << fileNum << " files indexed" << endl;
			return true;
		}else{
			dout << "FrameLoader::readStartIndexMap(): File does exist BUT is for a different filePrefix -> drop it" << endl;
			return false;
		}
	}else{
		dout << "SmallFrameLoader::readStartIndexMap(): File does not exist yet  ...." << endl;
		return false;
	}
}

void FrameLoader::storeStartIndexMap(){
	ofstream mapFile;
	stringstream mapFileName;
	mapFileName << string(tmpDir) << firstFrameNumber << string("_indexBigFrame.txt");
	dout << "FrameLoader::storeStartIndexMap(): Writing File: " << mapFileName.str() << endl;
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
		dout << "FrameLoader::storeStartIndexMap(): Opening failed  ...." << endl;
	}
}

void FrameLoader::readHalfFrameStartingFrom(FILE *f,mypack *p,unsigned int frameNumber){
  while(readHalfFrame(f,p) && p->framenum < frameNumber);
	  // cout << "FrameLoader::readHalfFrameStartingFrom: looking for frame " << frameNumber << " found frame " << p->framenum << endl;
  //}
	return;
}

void FrameLoader::readNextCompleteFrame(FILE *f, FullFrame *ff, mypack *firstHalf){
	mypack *p1;
	if(firstHalf == NULL){
		p1 = (mypack *)malloc(sizeof(mypack));
		readHalfFrame(f,p1);
	}else{
		p1 = firstHalf;
	}

	mypack *p2 = (mypack *)malloc(sizeof(mypack));
	readHalfFrame(f,p2);
	while(p2->framenum != p1->framenum+1 || p2->framenum % 2 != 0){
		//cout << "p2: " << p2->framenum << " p1: " << p1->framenum << endl;
		if(p1 != firstHalf){
			free(p1);
		}
		p1 = p2;
		p2 = (mypack *)malloc(sizeof(mypack));
		if(!readHalfFrame(f,p2)){
			dout << "FrameLoader::readNextCompleteFrame EOF reached" << endl;
			ff->frame1_number = -1;
			return;
		}
	}
	ff->frame1_number=p1->framenum;
	ff->frame2_number=p2->framenum;
	memcpy(ff->inar,p1->inar,sizeof(short int) * HALFNCH);
	memcpy(&ff->inar[HALFNCH],p2->inar,sizeof(short int) * HALFNCH);
	free(p2);
	if(p1 != firstHalf){
		free(p1);
	}
}



vector<FullFrame*> FrameLoader::readFrames(unsigned int startFrameNumber,unsigned int amountOfFullFrames){
	dout << "FrameLoader::readFrames: Start reading from: " << startFrameNumber << " will read " << amountOfFullFrames << " frames" << endl;
	vector<FullFrame *> v;
	FullFrame *ff, *firstff;
	int lastFrameNumber;
	if(amountOfFullFrames == 0){
		return v;
	}
	FILE *f = findFileWithFrame(startFrameNumber);
	if(f!=NULL){
		mypack p;
		readHalfFrameStartingFrom(f,&p,startFrameNumber);
		firstff = (FullFrame *)malloc(sizeof(FullFrame));
		readNextCompleteFrame(f,firstff,&p);
		v.push_back(firstff);
		lastFrameNumber = firstff->frame2_number;
		while(lastFrameNumber < startFrameNumber + amountOfFullFrames * 2){
			ff = (FullFrame *)malloc(sizeof(FullFrame));
			readNextCompleteFrame(f,ff);
			if(ff->frame1_number == -1){
				//open next file and try again to read frame
				fclose(f);
				int nextFrameNumber = lastFrameNumber + 2;
				dout << "EOF reached. Try to find file with frame number: " << nextFrameNumber << endl;
				f = findFileWithFrame(nextFrameNumber);
				if(f == NULL){
					cerr << "FrameLoader::readFrames: reached end of dataset while reading frames" << endl;
					return v;
				}
				readNextCompleteFrame(f,ff);
				dout << "Ok. First frame of new file is: " << ff->frame1_number << endl;
			}
			v.push_back(ff);
			lastFrameNumber = ff->frame2_number;
		}
		fclose(f);
	}else{
		cerr << "FrameLoader::readFrames: could not find frame with starting number: " << startFrameNumber << endl;
	}
	return v;
}

void FrameLoader::update(){
    dout << "FrameLoader::update: Okay ... will look for new data" << endl;
    readStartIndexMap();
	storeStartIndexMap();
}

FILE *FrameLoader::findFileWithFrame(unsigned int frameNumber){
	unsigned int lowerBoundary = 0; int fileNumber;

	if(frameNumber > lastFrameNumber){
		dout << "FrameLoader::findFileWithFrame: End of dataset reached" << endl;
		dout << "FrameLoader::findFileWithFrame: Will look for new data" << endl;
		readStartIndexMap();
		storeStartIndexMap();
		if(frameNumber > lastFrameNumber){
			cerr << "FrameLoader::findFileWithFrame: Reached end of dataset and now new data available ..." << endl;
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
	dout << "FrameLoader::findFileWithFrame: File " << fileNumber << " contains frame " << frameNumber << endl;
	return openFile(fileNumber);
}

vector<FullFrame*> FrameLoader::loadAll(){
	vector<FullFrame *> v;
	FullFrame *ff;
	int fileId = 0;
	FILE *f = openFile(fileId);
	while(f!=NULL){
		ff = (FullFrame *)malloc(sizeof(FullFrame));
		readNextCompleteFrame(f,ff);
		if(ff->frame1_number > 0){
			v.push_back(ff);
		}else{
			fileId++;
			fclose(f);
			f = openFile(fileId);
			delete(ff);
		}
	}
	return v;
}

FullFrame *FrameLoader::readFrame(unsigned int frameNumber){
	FullFrame *ff = NULL;
	mypack p;
	FILE *f = findFileWithFrame(frameNumber);
	if(f!=NULL){
		dout << "scanning file" << endl;
		readHalfFrameStartingFrom(f,&p,frameNumber);
		dout << "found something. framenumber is: " << p.framenum << " and should be: " << frameNumber <<  endl;
		ff = (FullFrame *)malloc(sizeof(FullFrame));
		readNextCompleteFrame(f,ff,&p);
		fclose(f);
	}else{
		cerr << "could not find frame with number: " << frameNumber << endl;
	}
	return ff;
}

FILE *FrameLoader::_findFileWithFrame(unsigned int frameNumber){
	int fileNumber = 1;
	int lastFrameNumber = firstFrameNumber;
	while(lastFrameNumber >= 0){
		int nextFrameNumber = readFirstFrameNumber(fileNumber);
		if(lastFrameNumber <= frameNumber && nextFrameNumber > frameNumber){
			//found frame
			return openFile(fileNumber - 1);
		}

		if(lastFrameNumber <= frameNumber && nextFrameNumber < 0){
			//last file. check if frame is in there
			dout << "File " << fileNumber - 1 << " is the last file. Look if frame is in there" << endl;
			FILE *lastFile = openFile(fileNumber - 1);
			mypack lastPacket;
			readHalfFrameStartingFrom(lastFile,&lastPacket,frameNumber);
			fclose(lastFile);
			if(lastPacket.framenum == frameNumber){
				dout << "ok found frame in last file" << endl;
				return openFile(fileNumber -1);
			}else{
				cerr << "Frame " << frameNumber << " does not exist." << endl;
				return NULL;
			}
		}


		lastFrameNumber = nextFrameNumber;
		fileNumber++;
	}
	dout << "could not find frame" << endl;
	return NULL;
}
