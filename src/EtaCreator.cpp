/*
 * EtaCreator.cpp
 *
 *  Created on: Oct 5, 2012
 *      Author: l_cartier
 */

#include "EtaCreator.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <math.h>

EtaCreator::EtaCreator(string *inFileSet, string *_outDirname, int _startCh, int _endCh, int _nFrames) :
		outDirname(_outDirname), npede_eta(4000),
		noiseThr(100.),cpSumThr(300.), cpSumThrUp(500.), bet(700),
		startCh(_startCh), endCh(_endCh),
		opAmpCorrFactor(0.0), nFrames(_nFrames), frame_bunch(50000), frameNoiseThr(20000.) {
	frameLoader = new FRAME_LOADER(inFileSet);
	initRootStuff();

}

EtaCreator::~EtaCreator() {
	delete outDirname;
	delete frameLoader;
}

void EtaCreator::createEta() {

	ofstream of;
	string testFile;
	testFile = (*outDirname) + "_test.txt";
	of.open(testFile.c_str());

#ifdef SAVE_CHINFO
	string *tmpFile = new string("tmp.root");
	TFile *f = new TFile(tmpFile->c_str(),"RECREATE");
	cout << "writing per channel histos in " << *tmpFile << endl;
#endif

	getIntensity(frameLoader,nFrames);

#ifdef SAVE_CHINFO
	cpHisto->Write();
	noisePFrame->Write();
	for(int i = 0; i < 20; i++){
		noisePFramePhoton[i]->Write();
	}
	cout << "writing noisePFrame histo" << endl;
	f->Close();
#endif

	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		hetch[i]->ComputeIntegral();
		for (int ibin=0;ibin<bet;ibin++) intetach[i]->SetBinContent(ibin,hetch[i]->GetIntegral()[ibin]);
	}

	etaTot->ComputeIntegral();
	for (int ibin=0;ibin<bet;ibin++) iEtaTot->SetBinContent(ibin,etaTot->GetIntegral()[ibin]);

	dumpRootAndDescFile();

}

void EtaCreator::fitPeaks(string *bunchname,ofstream *outFile,int step ,int bunch) {
	double mypar[3];
	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		hch[i]->Fit("gaus", "Q", "", -500, 500);
		TF1 *fitfun = hch[i]->GetFunction("gaus");

#ifdef SAVE_CHINFO
		stringstream name;
		name << "s" << step << "_b" << bunch << "_" << i;
		hch[i]->SetName(name.str().c_str());
		hch[i]->SetTitle(name.str().c_str());
		hch[i]->Write();
		name << "tot";
		hchtot[i]->SetName(name.str().c_str());
		hchtot[i]->SetTitle(name.str().c_str());
		hchtot[i]->Write();
#endif


		if (fitfun != NULL) {
			fitfun->GetParameters(mypar);
			runav[i] = runav[i] + mypar[1];
			runsigma[i] = mypar[2];
			hch[i]->Reset();
			//htot->Reset();
//			if(i < 40 && i > 30){
//				cout << "Ch: " << i << " runav: " << runav[i] << " mypar[1] " << mypar[1] << " sigma: " << mypar[2] << endl;
//			}
			if (outFile != NULL) {
				(*outFile) << i << "\t" << runav[i] << "\t" << mypar[1] << "\t" << mypar[2] << endl;
			}

//			if(fitMeanPlot != NULL){
//				fitMeanPlot->Fill(i,step * framesPerStep/frame_bunch + bunch,mypar[1]);
//			}

		} else {


			cout << "fit data is empty: entry:  channel " << i << " step " << step << " bunch " << bunch<< endl;
			runsigma[i] = 0.0;
			if (outFile != NULL) {
				(*outFile) << i << "\t" << runav[i] << "\t" << "FIT NOT POSSIBLE" << endl;
			}
			//hch[i]->Draw();
			//return;
		}

	}
}


void EtaCreator::getIntensity(FRAME_LOADER *sfl,int nFrames){

	unsigned int initReadFrames = 0;
	unsigned int nReadFrames = 0;
	int bunch_nr = 0;
	FFV frames;


	frames = sfl->readFrames(sfl->firstFrameNumber+sfl->offset,npede_eta);
	initReadFrames += frames.size();
	initAvg(frames);
	freeVector(frames);

	frames = sfl->readFrames(sfl->firstFrameNumber+sfl->offset,frame_bunch);

	for (std::vector<SFP>::iterator it = frames.begin(); it != frames.end(); ++it) {
		for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
			hch[i]->Fill(((*it)->inar[i]) - runav[i] - ((*it)->inar[i-1] - runav[i-1]) * opAmpCorrFactor);
		}
	}
	fitPeaks(&string(""),NULL,-1,0);

	freeVector(frames);


	for(unsigned int bunch_start = 0; bunch_start < nFrames; bunch_start+=frame_bunch){
#ifdef SMALL_FRAMES
		frames = sfl->readFrames(sfl->firstFrameNumber+sfl->offset+bunch_start,frame_bunch);
#else
		frames = sfl->readFrames(sfl->firstFrameNumber+sfl->offset+2*bunch_start,frame_bunch);
#endif

		for (std::vector<SFP>::iterator it = frames.begin(); it != frames.end(); ++it) {
			double corrChannel[MAX_CHANNEL_NUMBER];
			double frameNoise = 0.0;
			int photonsPerFrame = 0;
			for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
				corrChannel[i] = ((*it)->inar[i]) - runav[i] - ((*it)->inar[i-1] - runav[i-1]) * opAmpCorrFactor;
				double channelNoise = sqrt(((*it)->inar[i] - runav[i])*((*it)->inar[i] - runav[i]));
				frameNoise += channelNoise;
			}
			if(frameNoise<frameNoiseThr){
				nReadFrames++;
				for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
					double cAdc = corrChannel[i];
					hchtot[i]->Fill(((*it)->inar[i]));
					hch[i]->Fill(cAdc);

					if(cAdc > noiseThr){
						treatHit(corrChannel, i);
						photonsPerFrame++;
					}
				}
//				noisePFrame->Fill(frameNoise);
//				noisePFramePhoton[photonsPerFrame]->Fill(frameNoise);
			}
			//cout << "Framenoiseis: " << frameNoise << endl;
		}
		if(frames.size() == 0){
			cerr << "no more frames in files. Can not complete " << endl;
			break;
		}
		freeVector(frames);
		stringstream bunchname;
		bunchname << "S" << 0 << "_RF" << nReadFrames;
		fitPeaks(&(bunchname.str()),NULL,0,bunch_nr);
		bunch_nr++;

		cout << (double)bunch_start/(double)nFrames*100.0 << "%" << endl;

	}

	cout << "Read frames: " << nReadFrames+initReadFrames << " starting from: " << sfl->firstFrameNumber+sfl->offset << endl;
}

void EtaCreator::treatHit(double *ch, int channelNumber){
	double LcAdc = ch[channelNumber - 1];
	double McAdc = ch[channelNumber] 	;
	double RcAdc = ch[channelNumber + 1];
	double L,R;
	int etaNr;


#ifdef ETA3
	double cpSum = LcAdc + McAdc + RcAdc;
	double eta = (McAdc + 2 * RcAdc)/ cpSum;
	etaNr = channelNumber-1;
#else

	if(LcAdc > McAdc || RcAdc > McAdc){
		return;
	}

	hitDistribution->Fill(channelNumber);

	if(LcAdc>RcAdc){
		L = LcAdc;
		R = McAdc;
		etaNr = channelNumber-2;
	}else{
		L = McAdc;
		R = RcAdc;
		etaNr = channelNumber-1;
	}

	double cpSum = L+R;
	double eta = R/cpSum;

	cpHisto->Fill(L,R);
#endif
	if(cpSum>cpSumThr && cpSum<cpSumThrUp&& etaNr >= MIN_CHANNEL_NUMBER && etaNr < MAX_CHANNEL_NUMBER-2){
		hetch[etaNr]->Fill(eta);
		etaTot->Fill(eta);
	}
}

void EtaCreator::initAvg(FFV v) {
	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		runav[i] = 0.0;
		runsigma[i] = 0.0;
	}

	cout << "initial average is calculated with " << v.size() << " frames" << endl;

	for (std::vector<SFP>::iterator it = v.begin(); it != v.end(); ++it) {
		for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
			runav[i] = (runav[i] + ((*it)->inar[i]) / double(v.size()));
			double v = ((*it)->inar[i]);
			hchtot[i]->Fill(v);
			//cout << i << ": runav[i]: " << runav[i] << " ((*it)->inar[i]): " << ((*it)->inar[i]) << endl;

		}
	}

#ifdef SAVE_CHINFO
	cout << "AVG: ";
	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		cout << i << ":" <<runav[i] << "\t";
		stringstream name;
		name << "INIT_" << i;
		hchtot[i]->SetName(name.str().c_str());
		hchtot[i]->SetTitle(name.str().c_str());
		hchtot[i]->Write();
		hchtot[i]->Reset();
	}
	cout << endl;
#endif


}

void deleteFrame_EtaCreator(SFP f){
	free(f);
}

void EtaCreator::freeVector(FFV v) {
	for_each(v.begin(), v.end(), deleteFrame_EtaCreator);
	v.clear();
}

void EtaCreator::initRootStuff() {
	hetch=new TH1F*[CHANNEL_NUMBER];
	intetach=new TH1F*[CHANNEL_NUMBER];
	hch = new TH1F*[MAX_CHANNEL_NUMBER];
	hchtot = new TH1F*[MAX_CHANNEL_NUMBER];
	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		runav[i] = 0;
		stringstream hname;
		hname << "hch" << i;
		hch[i] = new TH1F(hname.str().c_str(), hname.str().c_str(), 701, -300.5, max_po);
		hname << "tot";
		hchtot[i] = new TH1F(hname.str().c_str(), hname.str().c_str(), 16001, -8000, 8000);
		stringstream eta;
		eta << "eta" << i;
#ifdef ETA3
		hetch[i] = new TH1F(eta.str().c_str(),eta.str().c_str(),bet,-0.3,2.3);
		eta << "I";
		intetach[i] = new TH1F(eta.str().c_str(),eta.str().c_str(),bet,-0.3,2.3);
#else
		hetch[i] = new TH1F(eta.str().c_str(),eta.str().c_str(),bet,-0.3,1.3);
		eta << "I";
		intetach[i] = new TH1F(eta.str().c_str(),eta.str().c_str(),bet,-0.3,1.3);
#endif
	}
	stringstream hdh;
	hdh << "hitDistribution";
	hitDistribution = new TH1F(hdh.str().c_str(), hdh.str().c_str(), MAX_CHANNEL_NUMBER, MIN_CHANNEL_NUMBER-.5, MAX_CHANNEL_NUMBER-.5);

//	fitMeanPlot = new TH2F("fitMeanPlot","fitMeanPlot",MAX_CHANNEL_NUMBER-MIN_CHANNEL_NUMBER,MIN_CHANNEL_NUMBER-0.5,MAX_CHANNEL_NUMBER-0.5,
//												numberOfSteps*framesPerStep/frame_bunch,-0.5,numberOfSteps*framesPerStep/frame_bunch-0.5);

	cpHisto = new TH2F("cpHistogram","cpHistogram", 1100,-100.,1000., 1100, -100.,1000.);

	runsigma = new double[MAX_CHANNEL_NUMBER+1];

	noisePFramePhoton = new TH1F*[20];
	for(int i = 0; i < 20; i++){
		stringstream hname;
		hname << "noisePerFrame" << i;
		noisePFramePhoton[i] = new TH1F(hname.str().c_str(), hname.str().c_str(), 10001,-0.5,20000.5);
	}

	noisePFrame = new TH1F("noisePerFrame","noisePerFrame", 10001,-0.5,20000.5);

#ifdef ETA3
	etaTot = new TH1F("etaTot", "etaTot",bet, -0.3,2.3);
	iEtaTot = new TH1F("iEtaTot", "iEtaTot",bet, -0.3,2.3);
#else
	etaTot = new TH1F("etaTot", "etaTot",bet, -0.3,1.3);
	iEtaTot = new TH1F("iEtaTot", "iEtaTot",bet, -0.3,1.3);
#endif
//		hname << "noReset";
//		hchNoReset[i] = new TH1F(hname.str().c_str(), hname.str().c_str(), 701, -100.5, max_po);
//		hname << "raw";
//		rawhchNoReset[i] = new TH1F(hname.str().c_str(), hname.str().c_str(), 16001, -0.5, 16000.5);
}

void EtaCreator::dumpRootAndDescFile(){
	string etaRoot;
	etaRoot = (*outDirname) + "_etaDesc.root";
	cout << "Writing File: " << etaRoot << endl;
	TFile *f = new TFile(etaRoot.c_str(),"RECREATE");
	hitDistribution->Write();
	etaTot->Write();
	iEtaTot->Write();

	//cout << "Start Channel is " << MIN_CHANNEL_NUMBER << " end Channel is " << MAX_CHANNEL_NUMBER << endl;

	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		//hch[i]->Write();
		hetch[i]->Write();
		hchtot[i]->Write();
		intetach[i]->Write();
	}
	f->Close();

	ofstream descFile;
	string etaDesc;
	etaDesc = (*outDirname) + "_etaDesc.txt";
	cout << "Writing File: " << etaDesc << endl;
	descFile.open(etaDesc.c_str());
	EtaDescriptor ed;
	ostringstream inputSet;
	inputSet << (*frameLoader->filePrefix) << ":" << frameLoader->offset;
	ed.flatFieldInputSet = new string(inputSet.str());
	ed.etaRootFile = &etaRoot;
	ed.startChannel = MIN_CHANNEL_NUMBER;
	ed.endChannel = MAX_CHANNEL_NUMBER;
	ed.pedestal = runav;
	ed.opAmpCorrectionFactor = opAmpCorrFactor;

	descFile << &ed;
	descFile.close();
}

TH1F **loadCumulativeEta(string *etaFileName){
	ifstream etaFile;
	etaFile.open(etaFileName->c_str());
	EtaDescriptor ed;
	etaFile >> &ed;
	cout << "Try to load root file: " << (*ed.etaRootFile) << endl;

	TH1F **hetch;
	hetch=new TH1F*[CHANNEL_NUMBER];

	TFile *rootFile = new TFile(ed.etaRootFile->c_str());

	for (int i =0; i < CHANNEL_NUMBER; i++) {
		stringstream etaName;
		etaName << "iEtaTot";
		hetch[i] = new TH1F();
		rootFile->GetObject(etaName.str().c_str(),hetch[i]);
		if(hetch[i]==NULL){
			cerr << "Could not laod eta with name " << etaName.str() << endl;
			cerr << "Eta Descriptor for RootFile " << (*ed.etaRootFile) << " corrupted" << endl;
			break;
		}
	}
	//rootFile->Close();
	return hetch;
}

TH1F **loadEta(string *etaFileName){
	ifstream etaFile;
	etaFile.open(etaFileName->c_str());
	EtaDescriptor ed;
	etaFile >> &ed;
	cout << "Try to load root file: " << (*ed.etaRootFile) << endl;
	return loadEta(&ed);
}

TH1F **loadEta(EtaDescriptor *ed){
	TH1F **hetch;
	hetch=new TH1F*[CHANNEL_NUMBER];

	TFile *rootFile = new TFile(ed->etaRootFile->c_str());

	for (int i = ed->startChannel; i < ed->endChannel; i++) {
		stringstream etaName;
		etaName << "eta"<< i << "I";
		hetch[i] = new TH1F();
		rootFile->GetObject(etaName.str().c_str(),hetch[i]);
		if(hetch[i]==NULL){
			cerr << "Could not laod eta with name " << etaName.str() << endl;
			cerr << "Eta Descriptor for RootFile " << (*ed->etaRootFile) << " corrupted" << endl;
			break;
		}
	}
	//rootFile->Close();
	return hetch;
}
