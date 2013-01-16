/*
 * FrameLoader.h
 *
 *  Created on: Oct 2, 2012
 *      Author: l_cartier
 */

#ifndef FRAMELOADER_H_
#define FRAMELOADER_H_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>
#include <cstdlib>

#include "gotthard_constants.h"
#include "Packet.h"
#include "FullFrame.h"

#define tmpDir "/tmp/GotthardFileSets_"
#define DEBUG

class FrameLoader {
    public:
        FrameLoader(std::string filePrefix, int trialNumber); 
        virtual ~FrameLoader();
        void update();

        void readFrame(int frameNumber, FullFrame& frame);
        void readFrames(int startFrameNumber, int amountOfFullFrames, std::vector<FullFrame>& frames);
        void loadAll(std::vector<FullFrame>& frames);

    private:
        void readNextCompleteFrame(std::ifstream& f, FullFrame& ff, Packet& firstHalf);
        bool openFile(int number, std::ifstream& f);
        int readFirstFrameNumber(int fileNumber);
        int readLastFrameNumber(int fileNumber);
        std::istream& readHalfFrame(std::istream &f, Packet& p);
        int createStartIndexMap(int fileNr=0);
        void storeStartIndexMap();
        bool readStartIndexMap();
        int findFileWithFrame(int frameNumber);
        void readHalfFrameStartingFrom(std::ifstream& f, Packet& p, int frameNumber);
        void analyzeFileSetName(std::string filePrefix, int trialNumber);

        std::string filePrefix_;
        int firstFrameNumber_;
        int lastFrameNumber_;
        int offset_;
        std::map<int, int> startIndexMap_;
        int isGuiFileSet_;
        std::ostream debugout_;
        int internalError_;
        int trialNumber_;
};

#endif /* FRAMELOADER_H_ */
