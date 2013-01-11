#include <string>
#include "EtaCreator.h"
#include "configurations.h"

using namespace std;

int main(int argc, char **argv) {
	cout << "VERSION: SVN_REV: " << SVN_REV << " CONFIG: "  << CONFIG << endl;

	if(argc == 2){
		string arg = string(argv[1]);
		if(arg == string("-v")){
			return 1;
		}
	}

	int nFrames = 200000;

	if(argc < 6){
		cerr << "Usage: " << argv[0] << " [INPUT_FILE_SET] [OUTPUT_FILE_SET] [START_CH] [END_CH] [OPAMP correction factor] <ARGUMENTS>" << endl;
		cout << "ARGUMENTS" << endl;
		cout << "-n [nFrames]\tNumber of Frames (default = " << nFrames << " )" << endl;
		return -1;
	}

	for(int i = 6; i < argc; i++){
		string arg = string(argv[i]);
		if(arg == string("-n")){
			istringstream(argv[i+1]) >> nFrames;
			i++;
		}
	}

	string inFileSet(argv[1]);
	string outDir(argv[2]);

	int startChannel, endChannel;
	double opAmpCorrFactor;

	istringstream ( string(argv[3]) ) >> startChannel;
	istringstream ( string(argv[4]) ) >> endChannel;
	istringstream ( string(argv[5]) ) >> opAmpCorrFactor;


	if(startChannel>= endChannel || startChannel > 1280 || endChannel > 1280){
		cerr << "Check start and end channel" << endl;
	}



	cout << "Input File: " << inFileSet << endl;
	cout << "Output File: " << outDir << endl;
	cout << "Start Channel: " << startChannel << endl;
	cout << "End Channel: " << endChannel << endl;
	cout << "Op Amp Correction Factor: " << opAmpCorrFactor << endl;
	cout << "Number of Frames: " << nFrames << endl;



	EtaCreator *ec = new EtaCreator(&inFileSet, &outDir, startChannel, endChannel, nFrames);
	ec->opAmpCorrFactor = opAmpCorrFactor;

	ec->noiseThr=NOISE_THR;
	ec->cpSumThr=CP_SUM_THR;
	ec->cpSumThrUp=CP_SUM_THR_UP;
	ec->frameNoiseThr=FRAME_NOISE_THR;

	ec->createEta();

	return 0;
}


