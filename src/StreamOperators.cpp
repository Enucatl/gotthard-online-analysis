/*
 * ImageHandel.cpp
 *
 *  Created on: Oct 3, 2012
 *      Author: l_cartier
 */

#include "ImageHandel.h"
#include "EtaDescriptor.h"
#include "FrameLoader.h"



ostream& operator<< (ostream& out, ImageHandel *ih ){
	//cout << "Image Handle stream operator called" << endl;
	out << ih->timestamp << "\t" << ih->startFrameIndex << "\t" << ih->numberOfSubFrames << "\t" << ih->expTime << "\t" << ih->period << "\t" << ih->comment;

	return out;
}

istream& operator>> (istream& in, ImageHandel *ih ){
	in >> ih->timestamp >> ih->startFrameIndex >> ih->numberOfSubFrames >> ih->expTime >> ih->period ;
	getline(in,ih->comment);
	return in;
}

ostream& operator<< (ostream& out, EtaDescriptor *ed ){
	out << "#DO NOT MODIFY" << endl;
	out << "INPUT FILE SET:" << *(ed->flatFieldInputSet) << endl;
	out << "ETA ROOT FILE:" << *(ed->etaRootFile) << endl;
	out << "START CHANNEL:" << ed->startChannel << endl;
	out << "END CHANNEL:" << ed->endChannel << endl;
	out << "OPAMP CORRECTION:" << ed->opAmpCorrectionFactor << endl;

	out << "PEDESTAL:";
	for(int i = 0; i < CHANNEL_NUMBER; i++){
		out << ed->pedestal[i] << "\t";
	}
	out << endl;

	return out;
}

istream& operator>> (istream& in, EtaDescriptor *ed ){
	string dump;
	std::getline(in,dump);
	std::getline(in,dump,':'); std::getline(in,*(ed->flatFieldInputSet));
	std::getline(in,dump,':'); std::getline(in,*(ed->etaRootFile));
	std::getline(in,dump,':'); in >> ed->startChannel;
	std::getline(in,dump,':'); in >> ed->endChannel;
	std::getline(in,dump,':'); in >> ed->opAmpCorrectionFactor;
	std::getline(in,dump,':');
	for(int i = 0; i < CHANNEL_NUMBER; i++){
		in >> ed->pedestal[i];
	}


	return in;
}


