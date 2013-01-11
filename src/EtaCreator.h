/*
 * EtaCreator.h
 *
 *  Created on: Oct 5, 2012
 *      Author: l_cartier
 */

#ifndef ETACREATOR_H_
#define ETACREATOR_H_

#include <fstream>
#include <string>
#include <TH1F.h>
#include <TF1.h>
#include <TFile.h>
#include <TH2F.h>

#include "EtaDescriptor.h"

#define SAVE_CHINFO
#define ETA3

#define etaDir "/home/l_cartier/psi/imageInterpolationSoftware/eta/"
#define bunchLenght 100000

#define max_po 600

using namespace std;

#ifdef SMALL_FRAMES
	#include "SmallFrameLoader.h"
	#define FRAME_LOADER SmallFrameLoader
	typedef SmallFrame* SFP;
	typedef vector<SFP> FFV;
#else
	#define FRAME_LOADER FrameLoader
	#include "FrameLoader.h"
	typedef FullFrame* SFP;
	typedef vector<SFP> FFV;
#endif

#define MAX_CHANNEL_NUMBER endCh
#define MIN_CHANNEL_NUMBER startCh

class EtaCreator {
public:
	EtaCreator(string *inFileSet, string *_outDirname, int _startCh, int _endCh, int nFrames);
	virtual ~EtaCreator();

	void createEta();

	int startCh, endCh;

	double opAmpCorrFactor;

	int npede_eta;
	double noiseThr;
	double cpSumThr;
	double cpSumThrUp;
	double frameNoiseThr;
	int bet;
	int nFrames;
	int frame_bunch;

	string *outDirname;
	FRAME_LOADER* frameLoader;

	TH1F **hch;   		// PH distribution
	TH1F **hchtot;
	TH1F *hitDistribution;
	TH1F **hetch;
	TH1F **intetach;
	TH1F *etaTot;
	TH1F *iEtaTot;

	TH1F *noisePFrame;
	TH1F **noisePFramePhoton;

	TH2F *cpHisto;

//	TH2F *fitMeanPlot;
	double *runsigma;

private:
	void initRootStuff();
	void dumpRootAndDescFile();
	void freeVector(FFV v);
	void initAvg(FFV v);

	void getIntensity(FRAME_LOADER *sfl,int nFrames);
	void fitPeaks(string *bunchname,ofstream *outFile,int step = 0,int bunch = 0);
	void treatHit(double *ch, int channelNumber);

	double runav[CHANNEL_NUMBER];
};

TH1F **loadEta(string *etaFileName);
TH1F **loadEta(EtaDescriptor *ed);
TH1F **loadCumulativeEta(string *etaFileName);

#endif /* ETACREATOR_H_ */
