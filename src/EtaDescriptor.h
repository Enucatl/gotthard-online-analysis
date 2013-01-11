/*
 * EtaDescriptor.h
 *
 *  Created on: Oct 9, 2012
 *      Author: l_cartier
 */

#ifndef ETADESCRIPTOR_H_
#define ETADESCRIPTOR_H_

#include <string>
#include "FrameLoader.h"
#include <ostream>
#include <iostream>

using namespace std;

class EtaDescriptor {
public:
	EtaDescriptor() {
		flatFieldInputSet = new string("");
		etaRootFile = new string("");
		pedestal = new double[CHANNEL_NUMBER];
	}

	~EtaDescriptor() {}

	string *flatFieldInputSet;
	int startChannel;
	int endChannel;
	string *etaRootFile;

	double *pedestal;
	double opAmpCorrectionFactor;

	friend ostream& operator<< (ostream& out, EtaDescriptor *ed );
	friend istream& operator>> (istream& in, EtaDescriptor *ed );

};




#endif /* ETADESCRIPTOR_H_ */
