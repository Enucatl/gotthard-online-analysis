/*
 * GotthardConfigurator.h
 *
 *  Created on: Sep 26, 2012
 *      Author: l_cartier
 */

#ifndef GOTTHARDCONFIGURATOR_H_
#define GOTTHARDCONFIGURATOR_H_

#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>


#define SCRIPT_DIR "./scripts/"
#define OUTDIR "/home/l_cartier/scratch/"

using namespace std;

class GotthardConfigurator {
public:
	GotthardConfigurator(){
		bFinHostname = new string("");
		receiverIp = new string("129.129.205.163");
		receiverMac = new string("00:09:B6:1E:F6:00");
		outfiles = new string(OUTDIR);
		subframes = 10000;
		expTime = 0.000001;
		period = 0.0001;
		currentFrameIndex = 0;

		startReceiver();
	}
	virtual ~GotthardConfigurator();

	void connect();
	bool isConnected();
	void startReceiver();
	void applyConfiguration();
	bool acquire();

	string *bFinHostname;
	string *receiverIp;
	string *receiverMac;
	string *outfiles;

	int subframes;
	double expTime;
	double period;

	long currentFrameIndex;

private:
	int executeScript(string cmd);
};

#endif /* GOTTHARDCONFIGURATOR_H_ */
