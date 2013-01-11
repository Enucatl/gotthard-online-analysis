/*
 * CreateSinogram.cpp
 *
 *  Created on: Oct 29, 2012
 *      Author: l_cartier
 */


#include <math.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TFile.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "EtaCreator.h"
#include "configurations.h"

using namespace std;
//#define ETA3
#define DEBUG

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

#define MAX_CHANNEL_NUMBER maxCh //64
#define MIN_CHANNEL_NUMBER minCh //2

int npede_eta = 8000;
int max_po_cs = 600;
int frame_bunch = 100000;
int nPhotons = 0;


double frameNoiseThr = FRAME_NOISE_THR;
double opAmpCorrFactor= OP_AMP_CORR;
double noiseThr = NOISE_THR;
double cpSumThr = CP_SUM_THR;
double cpSumThrUp = CP_SUM_THR_UP;

double *runav;
double *runsigma;
TH2F *sinogramPlot;
TH2F *iSinogramPlot;
TH2F *fitMeanPlot = NULL;
TH2F *cpHisto;
TH2F *intensityIPos;

TH1F **hch;   		// PH distribution
TH1F **hchtot;   		// PH distribution
TH1F *noisePFrame;
TH1F **noisePFramePhoton;
TH1F *iPosTotal;
TH1F *cpSumHisto;
double **sinogram;
double **iSinogram;
double *flatFieldInt;
double *iFlatFieldInt;

int framesPerStep;
int numberOfSteps;

double flatfieldFrames = 300000.0;

int maxCh = 64, minCh = 2;

string *outFile;
string *filePrefix;
string *ffFilePrefix;

TH1F **etas;
int iFactor = 5;

int doPeakAdjustment;
int doFlatfield;
int doInterpolation;
int singlePhoton;
ofstream *singlePhotonFile;


void getIntensity(FRAME_LOADER *sfl ,double *outStrip, int step, bool doPeakFit, double *iOutStrip);

void deleteFrame(SFP f) {
	free(f);
}

void freeVector(FFV v) {
	for_each(v.begin(), v.end(), deleteFrame);
	v.clear();
}

void initAvg(FFV v) {
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

#ifdef DEBUG
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

void fitPeaks(string *bunchname,ofstream *outFile,int step = 0,int bunch = 0) {
	double mypar[3];
	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		//hch[i]->Fit("gaus", "Q", "", -500, 500); //TOMCAT
		hch[i]->Fit("gaus", "Q", "", -500, 200); //CSAXS
		TF1 *fitfun = hch[i]->GetFunction("gaus");

#ifdef DEBUG
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

			if(fitMeanPlot != NULL){
				fitMeanPlot->Fill(i,step * framesPerStep/frame_bunch + bunch,mypar[1]);
			}

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

void treatHit(double *ch, int channelNumber, int step,double *iOutStrip, unsigned int frameNr){
	double LcAdc = ch[channelNumber - 1];
	double McAdc = ch[channelNumber] 	;
	double RcAdc = ch[channelNumber + 1];
	double L,R;
	int etaNr;

	if(LcAdc > McAdc || RcAdc > McAdc){
		return;
	}

#ifdef ETA3
	double cpSum = LcAdc + McAdc + RcAdc;
	double eta = (McAdc + 2 * RcAdc)/ cpSum / 2.0;
	etaNr = channelNumber-1;
#else
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

#endif

#ifdef DEBUG
	cpHisto->Fill(L,R);
	cpSumHisto->Fill(cpSum);
#endif

	if(cpSum>cpSumThr && cpSum < cpSumThrUp && etaNr >= MIN_CHANNEL_NUMBER && etaNr < MAX_CHANNEL_NUMBER-1  ){
		double ipos = etas[etaNr]->GetBinContent(etas[etaNr]->FindBin(eta));
//		if(ipos < 0.1){
//			cout << "ipos: " << ipos << " eta: " << eta << " etaNr: " << etaNr << " L: " << LcAdc << " M: " << McAdc << " R: " << RcAdc << endl;
//		}
//#ifdef ETA3
//		ipos *= 2.0;
//#endif
		int iStripBin = (int)(ipos*(double)iFactor);

		iOutStrip[etaNr*iFactor+iStripBin] += 1.0;
		nPhotons++;

		if(singlePhoton){
			(*singlePhotonFile) << frameNr << "\t" << etaNr << "\t" << etaNr + 1 << "\t" << (double)etaNr+ipos << endl;
		}

#ifdef DEBUG
		if(channelNumber != 30 && channelNumber != 31 && channelNumber != 32 && channelNumber != 33 && channelNumber != 62&& channelNumber != 63){
			iPosTotal->Fill(ipos);
//			if(ipos > 1.0){ipos -= 1.0;}
//			iPosTotalOv->Fill(ipos);
//			if(ipos > 0.48 && ipos < 0.5){
//				cout << "cNr: " << channelNumber << " L: " << L << " R: " << R << " etaNr: " << etaNr << " eta: " << eta << " ipos: " << ipos << endl;
//			}
		}
		intensityIPos->Fill(ipos,cpSum);
#endif
	}
}


void writeFiles(){
	string sinogramRoot;
	sinogramRoot = (*outFile) + "_sinogram.root";
	cout << "Writing File: " << sinogramRoot << endl;
	TFile *ff = new TFile(sinogramRoot.c_str(),"RECREATE");
	ff->cd();
	sinogramPlot->Write();

#ifdef DEBUG
	fitMeanPlot->Write();
	noisePFrame->Write();

	for(int i = 0; i < 20; i++){
		noisePFramePhoton[i]->Write();
	}
#endif

	if(doInterpolation){
		iSinogramPlot->Write();

#ifdef DEBUG
		iPosTotal->Write();
		cpSumHisto->Write();
		cpHisto->Write();
		intensityIPos->Write();
#endif

		FILE *ibin;
		string isinogramBin;
		isinogramBin = (*outFile) + "_isinogram.bin";
		cout << "Writing File: " << isinogramBin << endl;
		ibin = fopen(isinogramBin.c_str(),"wb");
		for(int step = 0; step < numberOfSteps; step++){
			for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER*iFactor; i++) {
				fwrite(&iSinogram[step][i],sizeof(double),1,ibin);
			}
		}
		fclose(ibin);

	}
	ff->Close();

	FILE *bin;
	string sinogramBin;
	sinogramBin = (*outFile) + "_sinogram.bin";
	cout << "Writing File: " << sinogramBin << endl;
	bin = fopen(sinogramBin.c_str(),"wb");
	for(int step = 0; step < numberOfSteps; step++){
	stringstream stepTxt;
	stepTxt << (*outFile) <<"_stp"<<step<< ".txt";
	cout << "Writing File: " << stepTxt.str() << endl;
	ofstream txtFile;
	txtFile.open(stepTxt.str().c_str());
	txtFile << "#Step" << "\t" << "channel" << "\t" << "intensity" << endl;
		for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
			fwrite(&sinogram[step][i],sizeof(double),1,bin);
			txtFile << step << "\t" << i << "\t" << sinogram[step][i] << endl;
		}
		txtFile.close();
	}
	fclose(bin);

	//write flatfiled normalization values
	FILE *ffavg;
	string ffavgBin;
	ffavgBin = (*outFile) + "_ffavg.bin";
	cout << "Writing File: " << ffavgBin << endl;
	ffavg = fopen(ffavgBin.c_str(),"wb");
	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		//cout  << flatFieldInt[i] << "\t";
		fwrite(&(flatFieldInt[i]),sizeof(double),1,ffavg);
	}
	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		//cout  << flatFieldInt[i] << "\t";
		fwrite(&(runav[i]),sizeof(double),1,ffavg);
	}
	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		//cout  << flatFieldInt[i] << "\t";
		fwrite(&(runsigma[i]),sizeof(double),1,ffavg);
	}
	fclose(ffavg);

	//write interpolated flatfield normalizaiton values
	FILE *iffavg;
	string iffavgBin;
	iffavgBin = (*outFile) + "_iffavg.bin";
	cout << "Writing File: " << iffavgBin << endl;
	iffavg = fopen(iffavgBin.c_str(),"wb");
	for (int i = 0; i < MAX_CHANNEL_NUMBER*iFactor; i++) {
		fwrite(&(iFlatFieldInt[i]),sizeof(double),1,iffavg);
	}
	fclose(iffavg);
}

void createSinogram() {
	FRAME_LOADER *sfl = new FRAME_LOADER(filePrefix);

	sinogramPlot = new TH2F("sinogramPlot","sinogramPlot",MAX_CHANNEL_NUMBER-MIN_CHANNEL_NUMBER,MIN_CHANNEL_NUMBER-0.5,MAX_CHANNEL_NUMBER-0.5,
												numberOfSteps,-0.5,numberOfSteps-0.5);

	iSinogramPlot = new TH2F("iSinogramPlot","iSinogramPlot",(MAX_CHANNEL_NUMBER-MIN_CHANNEL_NUMBER)*iFactor,MIN_CHANNEL_NUMBER-0.5,MAX_CHANNEL_NUMBER-0.5,
													numberOfSteps,-0.5,numberOfSteps-0.5);
#ifdef DEBUG
	fitMeanPlot = new TH2F("fitMeanPlot","fitMeanPlot",MAX_CHANNEL_NUMBER-MIN_CHANNEL_NUMBER,MIN_CHANNEL_NUMBER-0.5,MAX_CHANNEL_NUMBER-0.5,
												numberOfSteps*framesPerStep/frame_bunch,-0.5,numberOfSteps*framesPerStep/frame_bunch-0.5);

	cpHisto = new TH2F("cpHistogram","cpHistogram", 1100,-100.,1000., 1100, -100.,1000.);
#endif

	sinogram = new double*[numberOfSteps];
	for(int j = 0; j < numberOfSteps; j++){
		sinogram[j] = new double[MAX_CHANNEL_NUMBER];
		for (int i = 0; i < MAX_CHANNEL_NUMBER; i++) {
			sinogram[j][i] = 0.0;
		}
	}

	iSinogram = new double*[numberOfSteps];
	for(int j = 0; j < numberOfSteps; j++){
		iSinogram[j] = new double[MAX_CHANNEL_NUMBER*iFactor];
		for (int i = 0; i < MAX_CHANNEL_NUMBER*iFactor; i++) {
			iSinogram[j][i] = 0.0;
		}
	}


	for(int step = 0; step < numberOfSteps; step++){
		getIntensity(sfl,sinogram[step],step,doPeakAdjustment,iSinogram[step]);
	}

	for(int step = 0; step < numberOfSteps; step++){
		for (int i = 0; i < MAX_CHANNEL_NUMBER*iFactor; i++) {
			double ipos = (double)i/(double)iFactor+1.0/(double)iFactor/2.0;
			iSinogram[step][i]/=iFlatFieldInt[i];
			iSinogramPlot->Fill(ipos,step,iSinogram[step][i]);
		}
	}

	for(int step = 0; step < numberOfSteps; step++){
		for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
			sinogram[step][i]/=flatFieldInt[i];
			sinogramPlot->Fill(i,step,sinogram[step][i]);
		}
	}
}

void calcFlatField(){
	for(int i = 0; i < MAX_CHANNEL_NUMBER; i++){
		flatFieldInt[i] = 0.0;
	}

	for(int i = 0; i < MAX_CHANNEL_NUMBER*iFactor; i++){
		iFlatFieldInt[i] = 0.0;
	}

	FRAME_LOADER *sfl = new FRAME_LOADER(ffFilePrefix);

	int tFramesPerStep = framesPerStep;
	framesPerStep = (int)flatfieldFrames;
	cout << "calcFlatField(): will read " << framesPerStep << " frames for flatfield" << endl;
	getIntensity(sfl,flatFieldInt,0, true, iFlatFieldInt);
	framesPerStep = tFramesPerStep;

	int correctedBins = 0;
	for(int i = 0; i < MAX_CHANNEL_NUMBER*iFactor; i++){
		if(iFlatFieldInt[i] == 0.0){
			iFlatFieldInt[i] = 1.0;
			correctedBins++;
		}
	}
	cout << "Flatfield finished. " << correctedBins << " bins are 0 and now corrected to 1." << endl;

//	cout << "FLATFIELD:" << endl;
//	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
//		cout << flatFieldInt[i] << "\t";
//	}
//	cout << endl;
}

void getIntensity(FRAME_LOADER *sfl,double *outStrip, int step, bool doPeakFit, double *iOutStrip){

	unsigned int initReadFrames = 0;
	unsigned int nReadFrames = 0;
	nPhotons = 0;
	int bunch_nr = 0;
	FFV frames;

	if(step == 0){
		frames = sfl->readFrames(sfl->firstFrameNumber+sfl->offset,npede_eta);
		initReadFrames += frames.size();
		initAvg(frames);
		freeVector(frames);

#ifdef SMALL_FRAMES
		frames = sfl->readFrames(sfl->firstFrameNumber+sfl->offset,frame_bunch);
#else
		frames = sfl->readFrames(sfl->firstFrameNumber+sfl->offset,frame_bunch);
#endif
		for (std::vector<SFP>::iterator it = frames.begin(); it != frames.end(); ++it) {
			for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
				hch[i]->Fill(((*it)->inar[i]) - runav[i] - ((*it)->inar[i-1] - runav[i-1]) * opAmpCorrFactor);
			}
		}
		fitPeaks(&string(""),NULL,-1,0);

		freeVector(frames);
	}

	for(unsigned int bunch_start = 0; bunch_start < framesPerStep; bunch_start+=frame_bunch){
#ifdef SMALL_FRAMES
		frames = sfl->readFrames(sfl->firstFrameNumber+sfl->offset+(unsigned int)step*(unsigned int)framesPerStep+bunch_start,frame_bunch);
#else
		frames = sfl->readFrames(sfl->firstFrameNumber+sfl->offset+2*((unsigned int)step*(unsigned int)framesPerStep+bunch_start),frame_bunch);
#endif

		for (std::vector<SFP>::iterator it = frames.begin(); it != frames.end(); ++it) {
			double corrChannel[MAX_CHANNEL_NUMBER];
			double frameNoise = 0.0;
			int photonsPerFrame = 0;
			int frameNr;
#ifdef SMALL_FRAMES
			frameNr = (*it)->framenum;
#else
			frameNr = (*it)->frame1_number;
#endif
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
						outStrip[i] += cAdc;// / (double)framesPerStep;
						if(doInterpolation){
							treatHit(corrChannel, i, step,iOutStrip,frameNr);
						}
						photonsPerFrame++;
					}
				}
			}
#ifdef DEBUG
			noisePFrame->Fill(frameNoise);
			if(photonsPerFrame<20){
				noisePFramePhoton[photonsPerFrame]->Fill(frameNoise);
			}
#endif
		}
		if(frames.size() == 0){
			cerr << "no more frames in files. Can not complete step " << step << endl;
			break;
		}
		freeVector(frames);
		if(doPeakFit){
			stringstream bunchname;
			bunchname << "S" << step << "_RF" << nReadFrames;
			fitPeaks(&(bunchname.str()),NULL,step,bunch_nr);
			bunch_nr++;
		}
	}
#ifdef SMALL_FRAMES
	cout << "Step " << step << " read frames: " << nReadFrames+initReadFrames << " starting from: " << sfl->firstFrameNumber+sfl->offset+(unsigned int)(step-1)*(unsigned int)framesPerStep << " Photons: " << nPhotons << endl;
#else
	cout << "Step " << step << " read frames: " << nReadFrames+initReadFrames << " starting from: " << sfl->firstFrameNumber+sfl->offset+2*((unsigned int)(step-1)*(unsigned int)framesPerStep) << " Photons: " << nPhotons << endl;
#endif
	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		outStrip[i] /= (double)nReadFrames;
	}

//	for (int i = 0; i < MAX_CHANNEL_NUMBER*iFactor; i++) {
//		iOutStrip[i] /= (double)nReadFrames;
//	}

}

void loadFlatField(string *ffFile){
	ostringstream fffile;
	fffile << (*ffFile) << "_ffavg.bin";
    FILE *f = fopen(fffile.str().c_str(),"rb");

    for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		fread(&flatFieldInt[i],sizeof(double),1,f);
	}

    cout << "loaded " << MAX_CHANNEL_NUMBER << " values from " << fffile.str() << endl;

	ostringstream ifffile;
	ifffile << (*ffFile) << "_iffavg.bin";
    FILE *fi = fopen(ifffile.str().c_str(),"rb");

    for (int i = 0; i < MAX_CHANNEL_NUMBER*iFactor; i++) {
		fread(&iFlatFieldInt[i],sizeof(double),1,fi);
	}

    cout << "loaded " << MAX_CHANNEL_NUMBER*iFactor << " values from " << ifffile.str() << endl;
}

int main(int argc, char **argv){
	cout << "VERSION: SVN_REV: " << SVN_REV << " CONFIG: "  << CONFIG << endl;

	if(argc == 2){
		string arg = string(argv[1]);
		if(arg == string("-v")){
			return 1;
		}
	}

	if(argc < 5){
		cout << "Usage: " << argv[0] << " [steps] [frames_step] [inputFileSet] [outputPrefix] <ARGUMENTS>" << endl;
		cout << "ARGUMENTS" << endl;
		cout << "-f\t[flatfieldInputSet]\tFlatfield" << endl;
		cout << "-nf\t[frames]\t\tNumber of frames for flatfield. No effect without -f. (default = " << flatfieldFrames << ")" << endl;
		cout << "-l\t[flatfieldInputFile]\tLoad Flatfield Average from binary file. (no _ffavg.bin or _iffavg.bin extension)" << endl;
		cout << "-minCh\t[channel number]\tStarting channel. (default = " << minCh << ")" << endl;
		cout << "-maxCh\t[channel number]\tEnd channel. (default = " << maxCh << ")" << endl;
		cout << "-gi\t[frames]\t\tInterval for gauss fitting to adjust pedestal. (default = " << frame_bunch << ")" << endl;
		cout << "-eta\t[etaDescFile]\t\tEnable eta Interpolation" << endl;
		cout << "-etaC\t[etaDescFile]\t\tEnable eta Interpolation (with single cumulative eta distribution)" << endl;
		cout << "-iF\t[interpolation Factor]\tnumber of bins per channel. (default = " << iFactor << ")" << endl;
		cout << "-sp\t[single photon file]\tstore each hit in file." << endl;
		cout << "-np\t\t\t\tdisable peak adjustment" << endl;
		cout << "-v \t\t\t\tprint version" << endl;

		//cout << "Argc is " << argc <<  endl;
		return -1;
	}

	doPeakAdjustment = true;
	doFlatfield = false;
	doInterpolation = false;
	int useSingleEta = false;
	int loadFlatfield = false;
	string *ffFile;
	string *etaDescFile;
	string *singlePhotonFileString;


	for(int i = 5; i < argc; i++){
		string arg = string(argv[i]);
		if(arg == string("-f")){
			ffFilePrefix = new string(argv[i+1]);
			doFlatfield = true;
			i++;
		}

		if(arg == string("-l")){
			ffFile = new string(argv[i+1]);
			loadFlatfield = true;
			i++;
		}

		if(arg == string("-nf")){
			istringstream(argv[i+1]) >> flatfieldFrames;
			i++;
		}

		if(arg == string("-minCh")){
			istringstream(argv[i+1]) >> minCh;
			i++;
		}

		if(arg == string("-maxCh")){
			istringstream(argv[i+1]) >> maxCh;
			i++;
		}

		if(arg == string("-gi")){
			istringstream(argv[i+1]) >> frame_bunch;
			i++;
		}

		if(arg == string("-eta")){
			etaDescFile = new string(argv[i+1]);
			doInterpolation = true;
			i++;
		}

		if(arg == string("-etaC")){
			etaDescFile = new string(argv[i+1]);
			doInterpolation = true;
			useSingleEta = true;
			i++;
		}

		if(arg == string("-iF")){
			istringstream(argv[i+1]) >> iFactor;
			i++;
		}

		if(arg == string("-sp")){
			singlePhotonFileString = new string(argv[i+1]);
			singlePhoton = true;
			i++;
		}

		if(arg == string("-np")){
			doPeakAdjustment = false;
		}
	}

	if(doInterpolation){
		cout << "Will use etas from " << (*etaDescFile) << " for interpolation. Bins per Channel: " << iFactor << endl;
		if(useSingleEta){
			etas = loadCumulativeEta(etaDescFile);
		}else{
			etas = loadEta(etaDescFile);
		}

		iPosTotal = new TH1F("intrTot", "intrTot", 200, -0.05, 2.05);
		if(singlePhoton){
			singlePhotonFile = new ofstream();
			singlePhotonFile->open(singlePhotonFileString->c_str());
			(*singlePhotonFile) << "#frame\tLstrip\tRstrip\tpos" << endl;
		}
	}

	if(singlePhoton){
		cerr << "Please enable interpolation mode for single photon file." << endl;
		return -1;
	}


	istringstream(argv[1]) >> numberOfSteps;
	istringstream(argv[2]) >> framesPerStep;

	filePrefix = new string(argv[3]);
	outFile = new string(argv[4]);

	runav = new double[MAX_CHANNEL_NUMBER+1];
	runsigma = new double[MAX_CHANNEL_NUMBER+1];

#ifdef SMALL_FRAMES
	cout << "Using SmallFrames (256 channels)" << endl;
#else
	cout << "Using BigFrames (1280 channels)" << endl;
#endif

	cout << "Create sinogram with " << numberOfSteps << " steps and " << framesPerStep << " frames per step" << endl;
	cout << "Input File set is " << (*filePrefix) <<  endl;
	cout << "Output file set " << (*outFile) << endl;


	hch = new TH1F*[MAX_CHANNEL_NUMBER];
	hchtot = new TH1F*[MAX_CHANNEL_NUMBER];
	for (int i = MIN_CHANNEL_NUMBER; i < MAX_CHANNEL_NUMBER; i++) {
		stringstream hname;
		hname << "hch" << i;
		hch[i] = new TH1F(hname.str().c_str(), hname.str().c_str(), 1201, -max_po_cs, max_po_cs);
		hname << "tot";
		hchtot[i] = new TH1F(hname.str().c_str(), hname.str().c_str(), 16001, -8000, 8000);
	}

#ifdef DEBUG
	noisePFramePhoton = new TH1F*[20];
	for(int i = 0; i < 20; i++){
		stringstream hname;
		hname << "noisePerFrame" << i;
		noisePFramePhoton[i] = new TH1F(hname.str().c_str(), hname.str().c_str(), 10001,-0.5,10000000.5);
	}
	intensityIPos = new TH2F("intensityIPos","intensityIPos",iFactor,-0.1,2.1,100,-0.1,cpSumThrUp);
#endif


	noisePFrame = new TH1F("noisePerFrame","noisePerFrame", 10001,-0.5,10000000.5);
	cpSumHisto = new TH1F("cpSumHistogram","cpSumHistogram", 1001, -0.5, 1000.5);

	flatFieldInt = new double[MAX_CHANNEL_NUMBER];
	iFlatFieldInt = new double[MAX_CHANNEL_NUMBER*iFactor];

	if(doFlatfield){
		cout << "Take Flatfield from " << (*ffFilePrefix) << endl;
		calcFlatField();
	}else if(loadFlatfield){
		cout << "Load precalculated Flatfield from: " << (*ffFile) << endl;
		loadFlatField(ffFile);
	}else{
		cout << "No Flatfield" << endl;
		for(int i = 0; i < MAX_CHANNEL_NUMBER; i++){
			flatFieldInt[i] = 1.0;
		}
		for(int i = 0; i < MAX_CHANNEL_NUMBER*iFactor; i++){
			iFlatFieldInt[i] = 1.0;
		}
	}

	cout << "Start with sinogram ... " << endl;

#ifdef DEBUG
	string *tmpFile = new string("tmp.root");
	TFile *f = new TFile(tmpFile->c_str(),"RECREATE");
	cout << "writing per channel histos in " << *tmpFile << endl;
#endif

	createSinogram();
	writeFiles();

#ifdef DEBUG
	f->Close();
#endif
}
