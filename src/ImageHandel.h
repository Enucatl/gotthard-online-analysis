/*
 * ImageHandel.h
 *
 *  Created on: Oct 3, 2012
 *      Author: l_cartier
 */

#ifndef IMAGEHANDEL_H_
#define IMAGEHANDEL_H_

#include <ostream>
#include <iostream>
#include <ctime>


using namespace std;

class ImageHandel {
public:
	ImageHandel(int _startFrameIndex, int _numberOfSubFrames, double _expTime, double _period, string _comment) :
		startFrameIndex(_startFrameIndex),
		numberOfSubFrames(_numberOfSubFrames),
		expTime(_expTime),
		period(_period),
		comment(_comment) {
		timestamp = time(NULL);
	}


	ImageHandel(){};

	virtual ~ImageHandel() {}

	int startFrameIndex;
	int numberOfSubFrames;
	double expTime;
	double period;

	int timestamp;

	string comment;

	friend ostream& operator<< (ostream& out, ImageHandel *ih );
	friend istream& operator>> (istream& in, ImageHandel *ih );
};


#endif /* IMAGEHANDEL_H_ */
