/*
 * InterpolationMainModel.cpp
 *
 *  Created on: Oct 3, 2012
 *      Author: l_cartier
 */

#include "InterpolationMainModel.h"


void InterpolationMainModel::dumpImageHandels(){
	ofstream imageHandleFile;
	stringstream imageHandleFileName;
	imageHandleFileName << *(gotthardConfigurator->outfiles) + string("run_0_") << "AcquiredImages.txt";
	cout << "store Image Index at: " << imageHandleFileName.str() << endl;
	imageHandleFile.open(imageHandleFileName.str().c_str());

	for(std::vector<ImageHandel*>::iterator it = images.begin(); it != images.end(); ++it) {
		ImageHandel *ih = (*it);
		imageHandleFile << ih << endl;
		cout << "store Image Handle: " << ih << endl;
	}
	imageHandleFile.close();
}

void InterpolationMainModel::readImageHandels(){
	ifstream imageHandleFile;
	stringstream imageHandleFileName;
	imageHandleFileName << *(gotthardConfigurator->outfiles) + string("run_0_") << "AcquiredImages.txt";
	imageHandleFile.open(imageHandleFileName.str().c_str());

	cout << "read Image Index at: " << imageHandleFileName.str() << endl;

	if(imageHandleFile != NULL){
		images.empty();
		do{
			ImageHandel *ih = new ImageHandel();
			if(imageHandleFile >> ih){
				cout << "read Image Handle: " << ih << endl;
				images.push_back(ih);
			}
		}while(!imageHandleFile.eof());
	}

}

TH1 *InterpolationMainModel::getRawImage(){
	FrameLoader *fl = getFrameLoader();

	ImageHandel *ih = images.back();
	int startIndex = fl->firstFrameNumber + ih->startFrameIndex;
	cout << "creating raw Image for subframes starting at: " << startIndex << endl;
	vector<FullFrame *> subFrames = fl->readFrames(startIndex,ih->numberOfSubFrames);
	cout << "read " << subFrames.size() << " subframes from fileset" << endl;
	TH1 *h1 = imageInterpolator->getRawImage(ih,subFrames);
	return h1;
}

bool InterpolationMainModel::acquire(){
	if(gotthardConfigurator->acquire()){
		if(getFrameLoader() != NULL && getFrameLoader()->firstFrameNumber > 0){
			ImageHandel *h = new ImageHandel(
					gotthardConfigurator->currentFrameIndex,
					gotthardConfigurator->subframes,
					gotthardConfigurator->expTime,
					gotthardConfigurator->period,
					"(none)"
			);
			images.push_back(h);
			gotthardConfigurator->currentFrameIndex += gotthardConfigurator->subframes;
			return true;
		}else{
			cerr << "no data received from detector" << endl;
			return false;
		}
	}else{
		cerr << "acquire failed. Detector not idle" << endl;
		return false;
	}
}
