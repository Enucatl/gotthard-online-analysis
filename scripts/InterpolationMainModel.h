/*
 * InterpolationMainModel.h
 *
 *  Created on: Sep 26, 2012
 *      Author: l_cartier
 */

#ifndef INTERPOLATIONMAINMODEL_H_
#define INTERPOLATIONMAINMODEL_H_

#include "GotthardConfigurator.h"
#include <vector>
#include <iostream>
#include <sstream>
#include "ImageHandel.h"
#include "FrameLoader.h"
#include "ImageInterpolator.h"
#include <TH1.h>


#include <fstream>

using namespace std;

class InterpolationMainModel {
public:
	InterpolationMainModel() {
		gotthardConfigurator = new GotthardConfigurator();
		imageInterpolator = new ImageInterpolator();
		frameLoader = NULL;
		readImageHandels();
	}
	~InterpolationMainModel() {
		cout << "delete InterpolationMainModel" << endl;
		dumpImageHandels();
		delete gotthardConfigurator;
		if(frameLoader != NULL){
			delete frameLoader;
		}
	}

	GotthardConfigurator *gotthardConfigurator;
	vector<ImageHandel*> images;
	ImageInterpolator *imageInterpolator;

	FrameLoader *getFrameLoader(){
		if(frameLoader == NULL){
			string *fileset = new string(*(gotthardConfigurator->outfiles) + string("run_0_"));
			frameLoader = new FrameLoader(fileset);
			if(frameLoader->firstFrameNumber < 0){
				delete(frameLoader);
				frameLoader = NULL;
			}
		}
		return frameLoader;
	}

	TH1 *getRawImage();
	bool acquire();

private:
	FrameLoader *frameLoader;

	void dumpImageHandels();
	void readImageHandels();
};

#endif /* INTERPOLATIONMAINMODEL_H_ */
