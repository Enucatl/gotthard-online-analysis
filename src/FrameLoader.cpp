/*
 * FrameLoader.cpp
 *
 *  Created on: Oct 2, 2012
 *      Author: l_cartier
 */

#include "FrameLoader.h"

FrameLoader::FrameLoader(std::string filePrefix, int trialNumber):
debugout_(std::cout.rdbuf()) {
#ifndef DEBUG
    filebuf fb;
    fb.open("/dev/null");
    debugout_.rdbuf(&fb);
#endif

    analyzeFileSetName(filePrefix, trialNumber);

    internalError_ = 0;
    firstFrameNumber_ = readFirstFrameNumber(0);
    if(internalError_ < 0){
        std::cerr << "SmallFrameLoader: Can not open Dataset: " << filePrefix_ << " (int err=" << internalError_ << ")" << std::endl;
        firstFrameNumber_ = 0;
        lastFrameNumber_ = 0;
        return;
    }
    if(!readStartIndexMap()){
        std::cout << "recreating index ..." << std::flush;
        createStartIndexMap();
        storeStartIndexMap();
        std::cout << "done" << std::endl;
    }else{
        storeStartIndexMap();
    }

    debugout_ << "First Frame Number of files: " << filePrefix_ << "* is: " << firstFrameNumber_ << std::endl;
    debugout_ << "Last  Frame Number of files: " << filePrefix_ << "* is: " << lastFrameNumber_ << std::endl;
    debugout_ << "There are " << lastFrameNumber_ - firstFrameNumber_ << " frames (expect lost frames) in this file set" << std::endl;
    debugout_ << "Offset is " << offset_ << std::endl;  
    debugout_ << "Total files: " << startIndexMap_.size() << std::endl;
}                                 

FrameLoader::~FrameLoader() {
    debugout_ << "deconstructor of FrameLoader called" << std::endl;
}                       

void FrameLoader::analyzeFileSetName(std::string filePrefix, int trialNumber){
    long long int dataOffset = 0;
    trialNumber_ = trialNumber;
    filePrefix_ = filePrefix;

    debugout_ << "FrameLoader::analyzeFileSetName: without trial number " << filePrefix << std::endl;
    debugout_ << "FrameLoader::analyzeFileSetName: trial number is " << trialNumber << std::endl;

    std::istringstream withoutTrialNumberStream(filePrefix.c_str());

    std::getline(withoutTrialNumberStream, filePrefix, ':');
    withoutTrialNumberStream >> dataOffset;


    debugout_ << "FrameLoader::analyzeFileSetName: dataOffset is " << dataOffset << std::endl;
    debugout_ << "FrameLoader::analyzeFileSetName: filePrefix " << filePrefix << std::endl;


    if(dataOffset < 0){
        offset_ = lastFrameNumber_ + dataOffset - firstFrameNumber_;
    }
    else {
        offset_ = dataOffset;
    }

    if(trialNumber >= 0){
        debugout_ << "FrameLoader::analyzeFileSetName: enable isGuiFileSet_ mode" << std::endl;
        isGuiFileSet_ = true;
    }
    else {
        isGuiFileSet_ = false;
    }
}

int FrameLoader::readFirstFrameNumber(int fileNumber){
    std::ifstream firstFile;
    bool open_success = openFile(fileNumber, firstFile);
    int firstIndex = -1;
    if(!open_success){
        internalError_ = -1;
        return -1;
    }
    Packet p;
    if(readHalfFrame(firstFile, p)){
        firstIndex = p.frame_number();
    }
    else {
        internalError_ = -2;
        firstIndex = -2;
    }
    return firstIndex;
}

std::istream& FrameLoader::readHalfFrame(std::istream& f, Packet& p){
    return p.read_packet(f);
}

bool FrameLoader::openFile(int number, std::ifstream& f){
    std::stringstream filename;
    filename << filePrefix_;
    if(isGuiFileSet_){
        filename << "f";
        filename.fill('0');
        filename.width(12);
        filename << number * gotthard_constants::kFramesPerFile;
        filename << "_" << trialNumber_ << ".raw";
        debugout_ << "FrameLoader::openFile: Try to open file " << filename.str() << std::endl;
        f.open(filename.str().c_str(), std::ifstream::binary);
        if(!f.good()){
            debugout_ << "FrameLoader::openFile: can not open " << filename.str() << std::endl;
        }
    } else {
        filename.fill('0');
        filename.width(9);
        filename << number;
        filename << ".dat";
        debugout_ << "FrameLoader::openFile: Try to open file " << filename.str() << std::endl;
        f.open(filename.str().c_str(), std::ifstream::binary);
        if(!f.good()){
            debugout_ << "FrameLoader::openFile: *.dat doesnt work" << std::endl;
            std::stringstream new_filename;
            new_filename.fill('0');
            new_filename.width(9);
            new_filename << number;
            new_filename << ".raw";
            debugout_ << "FrameLoader::openFile: Try to open file " << new_filename.str() << std::endl;
            f.open(new_filename.str().c_str(), std::ifstream::binary);
            if(!f.good()){
                debugout_ << "FrameLoader::openFile: *.raw doesnt work" << std::endl;
            }
        }
    }
    return f.good();
}


int FrameLoader::createStartIndexMap(int fileNr){
    internalError_ = 0;
    int startFrameNumber = readFirstFrameNumber(fileNr);
    while(internalError_ >= 0){
        startIndexMap_[fileNr] = startFrameNumber;
        fileNr++;
        internalError_ = 0;
        startFrameNumber = readFirstFrameNumber(fileNr);

        if(fileNr % 100 == 0){
            debugout_ << "FrameLoader::createStartIndexMap: index for file Nr. " << fileNr << " startFrameNumber is : " << startFrameNumber << std::endl;
        }
    }
    lastFrameNumber_ = readLastFrameNumber(fileNr - 1);
    debugout_ << "FrameLoader::createStartIndexMap: done" << std::endl;
    return fileNr;
}

int FrameLoader::readLastFrameNumber(int fileNumber){
    debugout_ << "FrameLoader::readLastFrameNumber: find last frame in file: " << fileNumber << std::endl;
    int lfn = 0;
    std::ifstream lf;
    bool open_success = openFile(fileNumber, lf);
    if(open_success){
        lf.seekg(0, std::ifstream::end);
        int fs = lf.tellg();
        debugout_ << "FrameLoader::readLastFrameNumber: last file has length " << fs << std::endl;
        debugout_ << "FrameLoader::readLastFrameNumber: file length % 1286 = " << fs % 1286 << std::endl;
        debugout_ << "FrameLoader::readLastFrameNumber: will search at position " << fs - 1286 - (fs % 1286) << " for last frame number" << std::endl;
        lf.seekg(-1286 - (fs % 1286), std::ifstream::end);
        Packet lastFrame;
        if(readHalfFrame(lf, lastFrame).bad()){
            std::cerr << "could not read last frame in last file" << std::endl;
            lfn = -2;
            internalError_ = -2;
        } else {
            lfn = lastFrame.frame_number();
        }
    }
    else {
        std::cerr << "could not determine last frame_number()ber of fileSet" << std::endl;
        lfn = -1;
        internalError_ = -1;
    }
    debugout_ << "FrameLoader::readLastFrameNumber: done (" << lfn << ")" << std::endl;
    return lfn;
}

bool FrameLoader::readStartIndexMap(){
    std::ifstream mapFile;
    std::stringstream mapFileName;
    mapFileName << tmpDir << firstFrameNumber_ << "_indexBigFrame.txt";
    debugout_ << "FrameLoader::readStartIndexMap(): Try to open File: " << mapFileName << std::endl;
    mapFile.open(mapFileName.str().c_str());
    if(mapFile.is_open()){
        std::string dump, fileset;
        std::getline(mapFile, dump, ':');
        mapFile >> fileset;
        if(fileset == filePrefix_){
            startIndexMap_.clear();
            int fileNum;
            int frameNum;
            while(mapFile >> fileNum >> frameNum){
                startIndexMap_[fileNum] = frameNum;
            }
            debugout_ << "FrameLoader::readStartIndexMap(): read " << fileNum << " entries. startIndex of last: " << frameNum << std::endl;
            debugout_ << "FrameLoader::readStartIndexMap(): check for newly added files" << std::endl;
            fileNum = createStartIndexMap(fileNum + 1);
            debugout_ << "FrameLoader::readStartIndexMap(): now there are " << fileNum << " files indexed" << std::endl;
            return true;
        }
        else {
            debugout_ << "FrameLoader::readStartIndexMap(): File does exist BUT is for a different filePrefix -> drop it" << std::endl;
            return false;
        }
    }
    else {
        debugout_ << "SmallFrameLoader::readStartIndexMap(): File does not exist yet  ...." << std::endl;
        return false;
    }
}

void FrameLoader::storeStartIndexMap(){
    std::ofstream mapFile;
    std::stringstream mapFileName;
    mapFileName << tmpDir << firstFrameNumber_ << "_indexBigFrame.txt";
    debugout_ << "FrameLoader::storeStartIndexMap(): Writing File: " << mapFileName.str() << std::endl;
    mapFile.open(mapFileName.str().c_str());
    if(mapFile.is_open()){
        mapFile << "FILESET:" << filePrefix_ << std::endl;
        for(std::map<int, int>::iterator it = startIndexMap_.begin(); it != startIndexMap_.end(); it++){
            mapFile << it->first << "\t" << it->second << std::endl;
        }
        mapFile.close();
        std::string ARCmd("chmod 666 ");
        ARCmd += mapFileName.str();
        int result = system(ARCmd.c_str());
        if (result != 0) {
            std::cerr << "chmod failed " << mapFileName << std::endl;
        }

    }else{
        debugout_ << "FrameLoader::storeStartIndexMap(): Opening failed  ...." << std::endl;
    }
}

void FrameLoader::readHalfFrameStartingFrom(std::ifstream& f, Packet& p, int frameNumber) {
    while(readHalfFrame(f, p) && p.frame_number() < frameNumber);
}

void FrameLoader::readNextCompleteFrame(std::ifstream& f, FullFrame& ff, Packet& p1){
    ff.read_next_complete_frame(f, p1);
}

void FrameLoader::readFrames(int startFrameNumber, int amountOfFullFrames, std::vector<FullFrame>& frames){
    frames.reserve(amountOfFullFrames);
    startFrameNumber += firstFrameNumber_;
    debugout_ << "FrameLoader::readFrames: Start reading from: " << startFrameNumber << " will read " << amountOfFullFrames << " frames" << std::endl;
    if(amountOfFullFrames == 0){
        return;
    }
    std::ifstream f;
    int fileNumber = findFileWithFrame(startFrameNumber);
    bool open_success = openFile(fileNumber, f);
    if(open_success){
        Packet p;
        readHalfFrameStartingFrom(f, p, startFrameNumber);
        FullFrame firstff;
        readNextCompleteFrame(f, firstff, p);
        frames.push_back(firstff);
        lastFrameNumber_ = firstff.frame2_number();
        while(lastFrameNumber_ < startFrameNumber + amountOfFullFrames * 2){
            FullFrame ff;
            Packet empty_packet;
            readNextCompleteFrame(f, ff, empty_packet);
            if(ff.frame1_number() == -1){
                //open next file and try again to read frame
                f.close();
                int nextFrameNumber = lastFrameNumber_ + 2;
                debugout_ << "EOF reached. Try to find file with frame number: " << nextFrameNumber << std::endl;
                fileNumber = findFileWithFrame(nextFrameNumber);
                open_success = openFile(fileNumber, f);
                if(!open_success){
                    std::cerr << "FrameLoader::readFrames: reached end of dataset while reading frames" << std::endl;
                    return;
                }
                Packet another_empty_packet;
                readNextCompleteFrame(f, ff, another_empty_packet);
                debugout_ << "Ok. First frame of new file is: " << ff.frame1_number() << std::endl;
            }
            frames.push_back(ff);
            lastFrameNumber_ = ff.frame2_number();
        }
        f.close();
    }
    else {
        std::cerr << "FrameLoader::readFrames: could not find frame with starting number: " << startFrameNumber << std::endl;
    }
    return;
}

void FrameLoader::update(){
    debugout_ << "FrameLoader::update: Okay ... will look for new data" << std::endl;
    readStartIndexMap();
    storeStartIndexMap();
}

int FrameLoader::findFileWithFrame(int frameNumber){
    int lowerBoundary = 0;
    int fileNumber = 0;

    if(frameNumber > lastFrameNumber_){
        debugout_ << "FrameLoader::findFileWithFrame: End of dataset reached" << std::endl;
        debugout_ << "FrameLoader::findFileWithFrame: Will look for new data" << std::endl;
        readStartIndexMap();
        storeStartIndexMap();
        if(frameNumber > lastFrameNumber_){
            std::cerr << "FrameLoader::findFileWithFrame: Reached end of dataset and now new data available ..." << std::endl;
            abort();
            return -1;
        }
    }

    for(std::map<int, int>::iterator it = startIndexMap_.begin(); it != startIndexMap_.end(); ++it){
        int fileStartIndex = it->second;

        if(lowerBoundary < fileStartIndex && fileStartIndex <= frameNumber){
            lowerBoundary = fileStartIndex;
            fileNumber = it->first;
        }
    }
    debugout_ << "FrameLoader::findFileWithFrame: File " << fileNumber << " contains frame " << frameNumber << std::endl;
    return fileNumber;
}

void FrameLoader::loadAll(std::vector<FullFrame>& frames){
    int fileId = 0;
    std::ifstream f;
    bool open_success = openFile(fileId, f);
    while(open_success){
        FullFrame ff;
        Packet empty_packet;
        readNextCompleteFrame(f, ff, empty_packet);
        if(ff.frame1_number() > 0){
            frames.push_back(ff);
        }else{
            fileId++;
            f.close();
            open_success = openFile(fileId, f);
        }
    }
    return;
}

void FrameLoader::readFrame(int frameNumber, FullFrame& frame){
    Packet p;
    frameNumber += firstFrameNumber_;
    int fileNumber = findFileWithFrame(frameNumber);
    std::ifstream f;
    bool open_success = openFile(fileNumber, f);
    if(open_success){
        debugout_ << "scanning file" << std::endl;
        readHalfFrameStartingFrom(f, p, frameNumber);
        debugout_ << "found something. frame_number is: " << p.frame_number() << " and should be: " << frameNumber <<  std::endl;
        readNextCompleteFrame(f, frame, p);
        f.close();
    }
    else {
        std::cerr << "could not find frame with number: " << frameNumber << std::endl;
    }
    return;
}
