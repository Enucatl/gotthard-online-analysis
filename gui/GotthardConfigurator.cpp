/*
 * GotthardConfigurator.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: l_cartier
 */

#include "GotthardConfigurator.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <iomanip>



GotthardConfigurator::~GotthardConfigurator() {
	cout << "killall slsDetectorReceiver" << endl;
	system("killall slsDetectorReceiver");
}

void GotthardConfigurator::connect(){
	cout << "okay try to connect to " << *bFinHostname << endl;

	stringstream cmd;
	cmd << "connectGotthard " << *bFinHostname;

	int retValue = executeScript(cmd.str());
}

bool GotthardConfigurator::isConnected(){
	stringstream cmd;
	cmd << "checkConnection " << *bFinHostname;
	int retValue = executeScript(cmd.str());

	return !retValue;
}

int GotthardConfigurator::executeScript(string cmd){
	stringstream completeCommnad;
	completeCommnad << "cd "<< SCRIPT_DIR << "; ./" << cmd ;
	cout << "Execute Line: " << completeCommnad.str() << endl;

	unsigned int retValue = system(completeCommnad.str().c_str());
	int exitStatus = (retValue >> 8) & 255;
	//cout << "exitStatus is: " << exitStatus << endl;
	return exitStatus;
}

void GotthardConfigurator::startReceiver(){
	stringstream cmd;
	cmd << "startReceiver";
	int retValue = executeScript(cmd.str());
}

void GotthardConfigurator::applyConfiguration(){
	stringstream cmd;
	cmd << setprecision(9);
	cmd << "configGotthard " << subframes << " " << expTime << " " <<  period << " " << *outfiles << " " << *receiverIp << " " << *receiverMac;
	int retValue = executeScript(cmd.str());

}

bool GotthardConfigurator::acquire(){
	if(executeScript("isIdle")){
		executeScript("acquireFrame");
		while(!executeScript("isIdle")){
			cout << "waiting for detector" << endl;
		}
	}else{
		cout << "gotthard not idle" << endl;
		return false;
	}


}
