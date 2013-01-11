/*
 * ImageInterpolationSoftware.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: l_cartier
 */


#include <iostream>
#include "FrameLoader.h"
#include "SmallFrameLoader.h"
#include <ctime>

using namespace std;

bool checkIfSmallFrames(vector<SmallFrame *> v ){
	long long int lastFrame = 0;
	double diff = 0;
	for (std::vector<SmallFrame *>::iterator it = v.begin(); it != v.end(); ++it) {
		if(lastFrame == 0){
			lastFrame = (*it)->framenum;
		}else{
			long long int currentFrame = (*it)->framenum;
			long long int cDiff = currentFrame - lastFrame;
			if(cDiff!=(long long int)1){
				diff++;
			}
			lastFrame = currentFrame;
		}
	}
	cout << "diff: " << diff << " v.size(): " << v.size() << endl;
	if((double)diff > (double)v.size() * 0.2){
		return false;
	}
	return true;
}

string printFrames(unsigned int frames){
	stringstream s;
	s << frames << " (" << frames / 1000 << " kFrames, " << frames /1000000 << " mFrames) ";
	return s.str();
}

int main(int argc, char **argv) {
	cout << "VERSION: SVN_REV: " << SVN_REV  << endl;

	if(argc == 2){
		string arg = string(argv[1]);
		if(arg == string("-v")){
			return 1;
		}


		string *s = new string(argv[1]);
		FrameLoader *fl = new FrameLoader(s);
		if(!fl->isGuiFileSet){
			SmallFrameLoader *sfl = new SmallFrameLoader(s);
			vector<SmallFrame *> v = sfl->readFrames(sfl->firstFrameNumber, 1000);
			if(v.size() != 0 && checkIfSmallFrames(v)){
				cout << "This fileset contains SMALL frames! (Channel 0 - 255)" << endl;
				cout << "First Frame Number of files: " << *s << "* is: " << sfl->firstFrameNumber << endl;
				cout << "Last  Frame Number of files: " << *s << "* is: " << sfl->lastFrameNumber << endl;
				cout << "There are " << printFrames(sfl->lastFrameNumber-sfl->firstFrameNumber) << " frames (expect lost frames) in this file set" << endl;
				cout << "Total files: " << sfl->startIndexMap.size() << endl;
			}else{

				cout << "This fileset contains BIG frames! (Channel 0 - 1279)" << endl;
				cout << "First Frame Number of files: " << *s << "* is: " << fl->firstFrameNumber << endl;
				cout << "Last  Frame Number of files: " << *s << "* is: " << fl->lastFrameNumber << endl;
				cout << "There are " << printFrames(fl->lastFrameNumber-fl->firstFrameNumber) << " HALF frames (expect lost frames) in this file set" << endl;
				cout << "There are " << printFrames((fl->lastFrameNumber-fl->firstFrameNumber)/2) << " FULL frames (expect lost frames) in this file set" << endl;
				cout << "Total files: " << fl->startIndexMap.size() << endl;
			}
		}else{
			cout << "This fileset contains BIG frames! (Channel 0 - 1279) And is recorded with gui" << endl;
			cout << "First Frame Number of files: " << *s << "* is: " << fl->firstFrameNumber << endl;
			cout << "Last  Frame Number of files: " << *s << "* is: " << fl->lastFrameNumber << endl;
			cout << "There are " << printFrames(fl->lastFrameNumber-fl->firstFrameNumber) << " HALF frames (expect lost frames) in this file set" << endl;
			cout << "There are " << printFrames((fl->lastFrameNumber-fl->firstFrameNumber)/2) << " FULL frames (expect lost frames) in this file set" << endl;
			cout << "Total files: " << fl->startIndexMap.size() << endl;
		}
	}else{
		cerr << "Usage: me <fset>" << endl;
	}

	return 0;
}


