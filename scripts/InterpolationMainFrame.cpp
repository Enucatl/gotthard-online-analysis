/*
 * InterpolationMainFrame.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: l_cartier
 */

#include "InterpolationMainFrame.h"
#include "ImageHandel.h"
#include <TGButton.h>

InterpolationMainFrame::InterpolationMainFrame(const TGWindow *p, UInt_t w, UInt_t h, InterpolationMainModel *i) {
	TGMainFrame::TGMainFrame(p, w, h);
	interpolationMainModel = i;

	//------------CONNECT GROUP
	TGGroupFrame *connectGroupFrame = new TGGroupFrame(this, "Detector", kVerticalFrame);
	TGHorizontalFrame *connectFrame = new TGHorizontalFrame(connectGroupFrame, 800, 30);

	TGLabel *hNamelabel = new TGLabel(connectFrame, "Detector Hostname:");
	connectFrame->AddFrame(hNamelabel, new TGLayoutHints(kLHintsTop, 5, 5, 5, 5));

	hostnameEntry = new TGTextEntry(connectFrame, "bchip005", 1);
	connectFrame->AddFrame(hostnameEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 0, 5, 0, 5));
	hostnameEntry->MapWindow();
	hostnameEntry->Resize(200, 30);

	TGTextButton *connectButton = new TGTextButton(connectFrame, "&Connect");
	connectFrame->AddFrame(connectButton, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
	connectButton->Connect("Clicked()","InterpolationMainFrame",this,"connectToDetector()");

	connectGroupFrame->AddFrame(connectFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2));
	this->AddFrame(connectGroupFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2));

	//------------ IMAGE CONF GROUP
	TGGroupFrame *imageConfGroupFrame = new TGGroupFrame(this, "Image Configuration", kVerticalFrame);
	TGHorizontalFrame *imageConfFrame = new TGHorizontalFrame(imageConfGroupFrame, 800, 30);

	imageConfApplyButton = new TGTextButton(imageConfFrame, "&Apply");
	imageConfFrame->AddFrame(imageConfApplyButton, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
	imageConfApplyButton->Connect("Clicked()","InterpolationMainFrame",this,"applyConfiguration()");
	imageConfApplyButton->SetEnabled(false);

	acquireImageButton = new TGTextButton(imageConfFrame, "&Acquire Image");
	imageConfFrame->AddFrame(acquireImageButton, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
	acquireImageButton->Connect("Clicked()","InterpolationMainFrame",this,"acquireImage()");
	acquireImageButton->SetEnabled(false);

	imageConfGroupFrame->AddFrame(imageConfFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2));
	this->AddFrame(imageConfGroupFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2));

	//------------ RESULT GRROUP
	TGGroupFrame *resultGroupFrame = new TGGroupFrame(this, "Current Image", kVerticalFrame);
	TGHorizontalFrame *resultFrame = new TGHorizontalFrame(resultGroupFrame, 800, 400);

	resultCanvas = new TRootEmbeddedCanvas("ResultCanvas",resultFrame,200,200);
	resultFrame->AddFrame(resultCanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,10,10,10,1));

	resultGroupFrame->AddFrame(resultFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2));
	this->AddFrame(resultGroupFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2));

	this->SetWindowName("Interpolation Software");
	this->MapSubwindows();
	this->Resize(800, 600);
	this->MapWindow();
}

InterpolationMainFrame::~InterpolationMainFrame() {
	cout << "delete InterpolationMainFrame" << endl;
	delete hostnameEntry;
	delete interpolationMainModel;
	gApplication->Terminate(0);
}

void InterpolationMainFrame::connectToDetector(){
	string *hostname = new string(hostnameEntry->GetText());
	cout << "try to connect to detector: " << *hostname << endl;
	delete interpolationMainModel->gotthardConfigurator->bFinHostname;
	interpolationMainModel->gotthardConfigurator->bFinHostname = hostname;
	interpolationMainModel->gotthardConfigurator->connect();

	if(interpolationMainModel->gotthardConfigurator->isConnected()){
		imageConfApplyButton->SetEnabled(true);
	}
}

void InterpolationMainFrame::applyConfiguration(){
	if(interpolationMainModel->gotthardConfigurator->isConnected()){
		interpolationMainModel->gotthardConfigurator->applyConfiguration();
		imageConfApplyButton->SetEnabled(false);
		acquireImageButton->SetEnabled(true);
	}
}

void InterpolationMainFrame::acquireImage(){
	if(interpolationMainModel->acquire()){
		drawRawImage();
	}
}

void InterpolationMainFrame::drawRawImage(){
	TH1 *f1 = interpolationMainModel->getRawImage();
	f1->Draw();
	TCanvas *fCanvas = resultCanvas->GetCanvas();
	fCanvas->cd();
	fCanvas->Update();
}


