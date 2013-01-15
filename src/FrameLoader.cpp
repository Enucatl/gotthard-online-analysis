/*
 * FrameLoader.cpp
 *
 *  Created on: Oct 2, 2012
 *      Author: l_cartier
 */

#include "FrameLoader.h"
#include <iostream>
#include <cerrno>
#include <cstring>

FrameLoader::FrameLoader(string filePrefix, int trialNumber) {
#ifndef DEBUG
    ofstream *dd = new ofstream();
    dd->open("/dev/null");
    debugout_ = dd;
#else
    debugout_ = &cout;
#endif

    analyzeFileSetName(filePrefix, trialNumber);

    internalError_ = 0;
    firstFrameNumber_ = readFirstFrameNumber(0);
    if(internalError_ < 0){
        cerr << "SmallFrameLoader: Can not open Dataset: " << filePrefix_ << " (int err=" << internalError_ << ")" << endl;
        firstFrameNumber_ = 0;
        lastFrameNumber_ = 0;
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

    *debugout_ << "First Frame Number of files: " << filePrefix_ << "* is: " << firstFrameNumber_ << endl;
    *debugout_ << "Last  Frame Number of files: " << filePrefix_ << "* is: " << lastFrameNumber_ << endl;
    *debugout_ << "There are " << lastFrameNumber_ - firstFrameNumber_ << " frames (expect lost frames) in this file set" << endl;
    *debugout_ << "Offset is " << offset_ << endl;  
    *debugout_ << "Total files: " << startIndexMap_.size() << endl;
}                                 

FrameLoader::~FrameLoader() {
    *debugout_ << "deconstructor of FrameLoader called" << endl;
}                       

void FrameLoader::analyzeFileSetName(string filePrefix, int trialNumber){
    long long int dataOffset = 0;
    trialNumber_ = trialNumber;
    filePrefix_ = filePrefix;

    *debugout_ << "FrameLoader::analyzeFileSetName: without trial number " << filePrefix << endl;
    *debugout_ << "FrameLoader::analyzeFileSetName: trial number is " << trialNumber << endl;

    istringstream *withoutTrialNumberStream = new istringstream(filePrefix.c_str());

    std::getline(*withoutTrialNumberStream,filePrefix,':');
    (*withoutTrialNumberStream) >> dataOffset;


    *debugout_ << "FrameLoader::analyzeFileSetName: dataOffset is " << dataOffset << endl;
    *debugout_ << "FrameLoader::analyzeFileSetName: filePrefix " << filePrefix << endl;


    if(dataOffset < 0){
        offset_ = lastFrameNumber_ + dataOffset - firstFrameNumber_;
    }else{
        offset_ = dataOffset;
    }

    if(trialNumber >= 0){
        *debugout_ << "FrameLoader::analyzeFileSetName: enable isGuiFileSet_ mode" << endl;
        isGuiFileSet_ = true;
    }else{
        isGuiFileSet_ = false;
    }
}

int FrameLoader::readFirstFrameNumber(int fileNumber){
    FILE *firstFile = openFile(fileNumber);
    int firstIndex = -1;
    if(firstFile == NULL){
        internalError_ = -1;
        return -1;
    }
    mypack *p = new mypack;
    if(readHalfFrame(firstFile,p)){
        firstIndex = p->framenum;
    }else{
        internalError_ = -2;
        firstIndex = -2;
    }
    delete p;
    fclose(firstFile);
    return firstIndex;
}

bool FrameLoader::readHalfFrame(FILE *f,mypack *p){
    //cout << "try to read half frame" << endl;
    return fread(p, BUFFER_LENGTH, 1, f);
}

FILE *FrameLoader::openFile(int number){
    char filename[256];
    FILE *f;
    if(isGuiFileSet_){
        sprintf(filename, "%sf%012d_%i.raw", filePrefix_.c_str(), number *FRAMES_PER_FILE, trialNumber_);
        *debugout_ << "FrameLoader::openFile: Try to open file " << filename << endl;
        f = fopen((const char *) (filename), "r");
        if(f == NULL){
            *debugout_ << "FrameLoader::openFile: can not open " << filename << endl;
        }
    }else{
        sprintf(filename, "%s%09d.dat", filePrefix_.c_str(), number);
        *debugout_ << "FrameLoader::openFile: Try to open file " << filename << endl;
        f = fopen((const char *) (filename), "r");
        if(f == NULL){
            *debugout_ << "FrameLoader::openFile: *.dat doesnt work" << endl;
            filename[0] = '\0';
            sprintf(filename, "%s%09d.raw", filePrefix_.c_str(), number);
            *debugout_ << "FrameLoader::openFile: Try to open file " << filename << endl;
            f = fopen((const char *) (filename), "r");
            if(f == NULL && ENOENT != errno){
                *debugout_ << "FrameLoader::openFile: *.raw doesnt work" << endl;
                perror("error while opening file: ");
            }
        }
    }
    return f;
}


int FrameLoader::createStartIndexMap(int fileNr){
    internalError_ = 0;
    int startFrameNumber = readFirstFrameNumber(fileNr);
    while(internalError_>=0){
        startIndexMap_[fileNr] = startFrameNumber;
        fileNr++;
        internalError_ = 0;
        startFrameNumber = readFirstFrameNumber(fileNr);

        if(fileNr % 100 == 0){
            *debugout_ << "FrameLoader::createStartIndexMap: index for file Nr. " << fileNr << " startFrameNumber is : " << startFrameNumber << endl;
        }
    }
    lastFrameNumber_ = readLastFrameNumber(fileNr - 1);
    *debugout_ << "FrameLoader::createStartIndexMap: done" << endl;
    return fileNr;
}

int FrameLoader::readLastFrameNumber(int fileNumber){
    *debugout_ << "FrameLoader::readLastFrameNumber: find last frame in file: " << fileNumber << endl;
    int lfn = 0;
    FILE *lf = openFile(fileNumber);
    if(lf != NULL){
        fseek(lf, 0, SEEK_END);
        int fs = ftell(lf);
        *debugout_ << "FrameLoader::readLastFrameNumber: last file has length " << fs << endl;
        *debugout_ << "FrameLoader::readLastFrameNumber: file length % 1286 = " << fs % 1286 << endl;
        *debugout_ << "FrameLoader::readLastFrameNumber: will search at position " << fs - 1286 - (fs % 1286) << " for last frame number" << endl;
        fseek(lf, -1286 - (fs % 1286), SEEK_END);
        mypack lastFrame;
        if(readHalfFrame(lf, &lastFrame) < 1){
            cerr << "could not read last frame in last file" << endl;
            lfn = -2;
            internalError_ = -2;
        }else{
            lfn = lastFrame.framenum;
            //	printf("%i: last frame hex: 0x%x  %u\n",i,lfn,lfn);

        }
        //}
}else{
    cerr << "could not determine last framenumber of fileSet" << endl;
    lfn = -1;
    internalError_ = -1;
}
*debugout_ << "FrameLoader::readLastFrameNumber: done (" << lfn << ")" << endl;
return lfn;
}

bool FrameLoader::readStartIndexMap(){
    ifstream mapFile;
    stringstream mapFileName;
    mapFileName << string(tmpDir) << firstFrameNumber_ << string("_indexBigFrame.txt");
    *debugout_ << "FrameLoader::readStartIndexMap(): Try to open File: " << mapFileName.str() << endl;
    mapFile.open(mapFileName.str().c_str());
    if(mapFile.is_open()){
        string dump, fileset;
        std::getline(mapFile,dump,':'); mapFile >> fileset;
        if(fileset == filePrefix_){
            startIndexMap_.clear();
            int fileNum;
            int frameNum;
            while(mapFile >> fileNum >> frameNum){
                startIndexMap_[fileNum] = frameNum;

            }
            *debugout_ << "FrameLoader::readStartIndexMap(): read " << fileNum << " entries. startIndex of last: " << frameNum << endl;
            *debugout_ << "FrameLoader::readStartIndexMap(): check for newly added files" << endl;
            fileNum = createStartIndexMap(fileNum+1);
            *debugout_ << "FrameLoader::readStartIndexMap(): now there are " << fileNum << " files indexed" << endl;
            return true;
        }else{
            *debugout_ << "FrameLoader::readStartIndexMap(): File does exist BUT is for a different filePrefix -> drop it" << endl;
            return false;
        }
    }else{
        *debugout_ << "SmallFrameLoader::readStartIndexMap(): File does not exist yet  ...." << endl;
        return false;
    }
}

void FrameLoader::storeStartIndexMap(){
    ofstream mapFile;
    stringstream mapFileName;
    mapFileName << string(tmpDir) << firstFrameNumber_ << string("_indexBigFrame.txt");
    *debugout_ << "FrameLoader::storeStartIndexMap(): Writing File: " << mapFileName.str() << endl;
    mapFile.open(mapFileName.str().c_str());
    if(mapFile.is_open()){
        mapFile << "FILESET:" << filePrefix_ << endl;
        for(map<int, int>::iterator it = startIndexMap_.begin(); it != startIndexMap_.end(); it++){
            mapFile << it->first << "\t" << it->second << endl;
        }
        mapFile.close();

        string ARCmd = "chmod 666 ";
        ARCmd += mapFileName.str();
        int result = system(ARCmd.c_str());
        if (result != 0) {
            cerr << "chmod failed " << mapFileName << endl;
        }

    }else{
        *debugout_ << "FrameLoader::storeStartIndexMap(): Opening failed  ...." << endl;
    }
}

void FrameLoader::readHalfFrameStartingFrom(FILE *f,mypack *p, int frameNumber){
    while(readHalfFrame(f,p) && p->framenum < frameNumber);
    // cout << "FrameLoader::readHalfFrameStartingFrom: looking for frame " << frameNumber << " found frame " << p->framenum << endl;
    //}
    return;
    }

void FrameLoader::readNextCompleteFrame(FILE *f, FullFrame *ff, mypack *firstHalf){
    mypack *p1;
    if(firstHalf == NULL){
        p1 = new mypack;
        readHalfFrame(f,p1);
    }else{
        p1 = firstHalf;
    }

    mypack *p2 = new mypack;
    readHalfFrame(f,p2);
    while(p2->framenum != p1->framenum+1 || p2->framenum % 2 != 0){
        if(p1 != firstHalf){
            delete p1;
        }
        p1 = p2;
        p2 = new mypack;
        if(!readHalfFrame(f,p2)){
            *debugout_ << "FrameLoader::readNextCompleteFrame EOF reached" << endl;
            ff->frame1_number = -1;
            return;
        }
    }
    ff->frame1_number=p1->framenum;
    ff->frame2_number=p2->framenum;
    memcpy(ff->inar,p1->inar,sizeof(short int) * HALFNCH);
    memcpy(&ff->inar[HALFNCH],p2->inar,sizeof(short int) * HALFNCH);
    delete p2;
    if(p1 != firstHalf){
        delete p1;
    }
}



vector<FullFrame*> FrameLoader::readFrames(int startFrameNumber, int amountOfFullFrames){
    *debugout_ << "FrameLoader::readFrames: Start reading from: " << startFrameNumber << " will read " << amountOfFullFrames << " frames" << endl;
    vector<FullFrame *> v;
    FullFrame *ff, *firstff;
    if(amountOfFullFrames == 0){
        return v;
    }
    FILE *f = findFileWithFrame(startFrameNumber);
    if(f!=NULL){
        mypack p;
        readHalfFrameStartingFrom(f, &p, startFrameNumber);
        firstff = new FullFrame;
        readNextCompleteFrame(f, firstff, &p);
        v.push_back(firstff);
        lastFrameNumber_ = firstff->frame2_number;
        while(lastFrameNumber_ < startFrameNumber + amountOfFullFrames * 2){
            ff = new FullFrame;
            readNextCompleteFrame(f, ff);
            if(ff->frame1_number == -1){
                //open next file and try again to read frame
                fclose(f);
                int nextFrameNumber = lastFrameNumber_ + 2;
                *debugout_ << "EOF reached. Try to find file with frame number: " << nextFrameNumber << endl;
                f = findFileWithFrame(nextFrameNumber);
                if(f == NULL){
                    cerr << "FrameLoader::readFrames: reached end of dataset while reading frames" << endl;
                    return v;
                }
                readNextCompleteFrame(f, ff);
                *debugout_ << "Ok. First frame of new file is: " << ff->frame1_number << endl;
            }
            v.push_back(ff);
            lastFrameNumber_ = ff->frame2_number;
        }
        fclose(f);
    }else{
        cerr << "FrameLoader::readFrames: could not find frame with starting number: " << startFrameNumber << endl;
    }
    return v;
}

void FrameLoader::update(){
    *debugout_ << "FrameLoader::update: Okay ... will look for new data" << endl;
    readStartIndexMap();
    storeStartIndexMap();
}

FILE *FrameLoader::findFileWithFrame(int frameNumber){
    int lowerBoundary = 0;
    int fileNumber = 0;

    if(frameNumber > lastFrameNumber_){
        *debugout_ << "FrameLoader::findFileWithFrame: End of dataset reached" << endl;
        *debugout_ << "FrameLoader::findFileWithFrame: Will look for new data" << endl;
        readStartIndexMap();
        storeStartIndexMap();
        if(frameNumber > lastFrameNumber_){
            cerr << "FrameLoader::findFileWithFrame: Reached end of dataset and now new data available ..." << endl;
            abort();
            return NULL;
        }
    }

    for(map<int, int>::iterator it = startIndexMap_.begin(); it != startIndexMap_.end(); it++){
        int fileStartIndex = it->second;

        if(lowerBoundary < fileStartIndex && fileStartIndex <= frameNumber){
            lowerBoundary = fileStartIndex;
            fileNumber = it->first;
        }
    }
    *debugout_ << "FrameLoader::findFileWithFrame: File " << fileNumber << " contains frame " << frameNumber << endl;
    return openFile(fileNumber);
}

vector<FullFrame*> FrameLoader::loadAll(){
    vector<FullFrame *> v;
    FullFrame *ff;
    int fileId = 0;
    FILE *f = openFile(fileId);
    while(f!=NULL){
        ff = new FullFrame;
        readNextCompleteFrame(f, ff);
        if(ff->frame1_number > 0){
            v.push_back(ff);
        }else{
            fileId++;
            fclose(f);
            f = openFile(fileId);
            delete ff;
        }
    }
    return v;
}

FullFrame *FrameLoader::readFrame(int frameNumber){
    FullFrame *ff = NULL;
    mypack p;
    FILE *f = findFileWithFrame(frameNumber);
    if(f != NULL){
        *debugout_ << "scanning file" << endl;
        readHalfFrameStartingFrom(f, &p, frameNumber);
        *debugout_ << "found something. framenumber is: " << p.framenum << " and should be: " << frameNumber <<  endl;
        ff = new FullFrame;
        readNextCompleteFrame(f, ff, &p);
        fclose(f);
    }else{
        cerr << "could not find frame with number: " << frameNumber << endl;
    }
    return ff;
}

FILE *FrameLoader::_findFileWithFrame(int frameNumber){
    int fileNumber = 1;
    lastFrameNumber_ = firstFrameNumber_;
    while(lastFrameNumber_ >= 0){
        int nextFrameNumber = readFirstFrameNumber(fileNumber);
        if(lastFrameNumber_ <= frameNumber && nextFrameNumber > frameNumber){
            //found frame
            return openFile(fileNumber - 1);
        }

        if(lastFrameNumber_ <= frameNumber && nextFrameNumber < 0){
            //last file. check if frame is in there
            *debugout_ << "File " << fileNumber - 1 << " is the last file. Look if frame is in there" << endl;
            FILE *lastFile = openFile(fileNumber - 1);
            mypack lastPacket;
            readHalfFrameStartingFrom(lastFile,&lastPacket,frameNumber);
            fclose(lastFile);
            if(lastPacket.framenum == frameNumber){
                *debugout_ << "ok found frame in last file" << endl;
                return openFile(fileNumber -1);
            }else{
                cerr << "Frame " << frameNumber << " does not exist." << endl;
                return NULL;
            }
        }


        lastFrameNumber_ = nextFrameNumber;
        fileNumber++;
    }
    *debugout_ << "could not find frame" << endl;
    return NULL;
}
