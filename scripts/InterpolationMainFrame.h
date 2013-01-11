/*
 * InterpolationMainFrame.h
 *
 *  Created on: Sep 26, 2012
 *      Author: l_cartier
 */

#ifndef INTERPOLATIONMAINFRAME_H_
#define INTERPOLATIONMAINFRAME_H_


#include <TQObject.h>
#include <RQ_OBJECT.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <string>
#include <iostream>
#include "InterpolationMainModel.h"
#include <TGApplication.h>
#include <TRootEmbeddedCanvas.h>
#include <TF1.h>

#include <TCanvas.h>

class TGWindow;
class TGMainFrame;
class TRootEmbeddedCanvas;

using namespace std;

class InterpolationMainFrame : public TGMainFrame{
RQ_OBJECT("InterpolationMainFrame")
public:
	InterpolationMainFrame(const TGWindow *p, UInt_t w, UInt_t h, InterpolationMainModel *i);
	virtual ~InterpolationMainFrame();

	void connectToDetector();
	void applyConfiguration();
	void acquireImage();
	void drawRawImage();

	ClassDef(InterpolationMainFrame,0)
private:
	TGTextEntry *hostnameEntry;
	InterpolationMainModel *interpolationMainModel;
	TGTextButton *imageConfApplyButton;
	TGTextButton *acquireImageButton;
	TRootEmbeddedCanvas *resultCanvas;
};

#endif /* INTERPOLATIONMAINFRAME_H_ */
