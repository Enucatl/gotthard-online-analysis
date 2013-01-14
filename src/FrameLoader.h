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
#include <sstream>
#include <vector>
#include <map>
#include <fstream>

const int BUFFER_LENGTH = 1286;
const int HALFNCH = 640;
const int CHANNEL_NUMBER = 1280;
const int FRAMES_PER_FILE = 20000;

#define tmpDir "/tmp/GotthardFileSets_"
#define DEBUG

struct mypack {
    // char someheader[];
    int framenum;
    unsigned short inar[HALFNCH+1];
    //unsigned short space;
};

struct FullFrame {
    int frame1_number;
    int frame2_number;
    unsigned short inar[CHANNEL_NUMBER];
};

class FrameLoader {
    public:
        FrameLoader(std::string filePrefix, int trialNumber); 
        virtual ~FrameLoader();
        void update();

        std::vector<FullFrame*> readFrames(int startFrameNumber, int amountOfFullFrames);
        FullFrame *readFrame(int frameNumber);
        std::vector<FullFrame*> loadAll();

    private:
        void readNextCompleteFrame(FILE *f, FullFrame* ff, mypack*firstHalf=NULL);
        FILE *openFile(int number);
        int readFirstFrameNumber(int fileNumber);
        int readLastFrameNumber(int fileNumber);
        bool readHalfFrame(FILE *f, mypack *p);
        int createStartIndexMap(int fileNr = 0);
        void storeStartIndexMap();
        bool readStartIndexMap();
        FILE *findFileWithFrame(int frameNumber);
        FILE *_findFileWithFrame(int frameNumber);
        void readHalfFrameStartingFrom(FILE *f, mypack *p, int frameNumber);
        void analyzeFileSetName(std::string filePrefix, int trialNumber);

        std::string filePrefix_;
        int firstFrameNumber_;
        int lastFrameNumber_;
        int offset_;
        std::map<int, int> startIndexMap_;
        int isGuiFileSet_;
        ostream *debugout_;
        int internalError_;
        int trialNumber_;
};

#endif /* FRAMELOADER_H_ */
