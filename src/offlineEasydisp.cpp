/*
 * offlineEasydisp.cpp
 *
 *  Created on: Nov 15, 2012
 *      Author: l_cartier
 */

#include <TROOT.h>
#include <TRint.h>
#include <TH1.h>
#include <TH2.h>
#include <fstream>
#include <TCanvas.h>
#include <TMath.h>
#include <TList.h>
#include <TPad.h>
#include <TServerSocket.h>
#include <TSocket.h>
#include <iostream>
#include <TApplication.h>

//#include <easydisp.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>  /* exit() */
#include <string.h>  /* memset(), memcpy() */
#include <sys/utsname.h>   /* uname() */
#include <sys/types.h>
#include <sys/socket.h>   /* socket(), bind(),
			     listen(), accept() */
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>  /* fork(), write(), close() */
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <asm/page.h>
#include<curses.h>

#include "SmallFrameLoader.h"
#include "FrameLoader.h"

//#define OK 0
#define FAIL -1

/**************************************************************************/
/* Constants used by this program                                         */
/**************************************************************************/
#define BUFFER_LENGTH    1286
#define FALSE              0
#define MAX_CH_SHOW 1280
#define MIN_CH_SHOW 0
#define MAX_POS 2000
#define MIN_POS -100

#define NB_ENABLE 1
#define NB_DISABLE 2



int main(int argc, char* argv[]) {
	if (argc < 2) {
#ifdef SMALL_FRAMES
		cout << "For SMALL FRAMES!!!" << endl;
#else
		cout << "For BIG FRAMES!!!" << endl;
#endif
		cout << "Usage: me [FILESTET]<:[OFFSET]> <channelNumber>" << endl;
		cout << "          [FILESTET] filename of first *.dat file without _00...0.dat" << endl;
		cout << "          [OFFSET] can be positive (start from begin) or negative (start from end of dataset)" << endl;
		return 0;
	}

	cout << "VERSION: SVN_REV: " << SVN_REV  << endl;

	string arg = string(argv[1]);
	if(arg == string("-v")){
		return 1;
	}

	string *fileset = new string(argv[1]);
	int si , bi, bii;
	bi = 100000;
#ifdef SMALL_FRAMES
	SmallFrameLoader *sfl = new SmallFrameLoader(fileset);
	si = 1;  bii = bi;
	SmallFrame *firstp;
	#define NCH 256
	#define HALFNCH 128
#else
	FrameLoader *sfl = new FrameLoader(fileset);
	si = 2;  bii = 2* bi;
	FullFrame *firstp;
	#define NCH 1280
	#define HALFNCH 640
#endif


	int sd = -1, rc1, rc2, rc;

	TApplication* rootapp = new TApplication("example", &argc, argv);

	TH1F* his1 = new TH1F("his1", "strip profile", NCH, -0.5, NCH - 0.5);

	TH1F* hchtot = new TH1F("hchtot", "ph tot", 300, MIN_POS, MAX_POS);

	TH1F* his2 = new TH1F("his2", "strip profile pede", NCH, -0.5, NCH - 0.5);
	TH1F* his3 = new TH1F("his3", "strip profile pede even ", NCH, -0.5, NCH - 0.5);
	TH1F* his4 = new TH1F("his4", "strip profile pede odd ", NCH, -0.5, NCH - 0.5);

	TH2F* his45 = new TH2F("his45", "L vs R", 100, MIN_POS, MAX_POS, 100, MIN_POS, MAX_POS);
	TCanvas *A2 = new TCanvas("A2", "Plotting Canvas 2", 50, 50, 400, 400);

	TCanvas *A3 = new TCanvas("A3", "Plotting Canvas 2", 150, 10, 900, 650);
	A3->Divide(1, 3);
	TPad *A3_1 = (TPad*) A3->GetListOfPrimitives()->FindObject("A3_1");

	TPad *A3_2 = (TPad*) A3->GetListOfPrimitives()->FindObject("A3_2");

	TPad *A33 = (TPad*) A3->GetListOfPrimitives()->FindObject("A3_3");
	A33->cd();

	A3->cd();
	int irun, i;
	double runav[NCH], dsvalue[NCH];
	double peso;
	std::ofstream filepede;
	TPad* aatop;
	TPad* aabot;
	double ytime = 0;
	irun = 0;
	bool doped;
	struct mypack {

		// char someheader[];
		int framenum;

		unsigned short inar[NCH];

	};
	for (i = 0; i < NCH; i++)
		runav[i] = 0.0;

	int chidx = 1;




	int cFrames = 0;

	if (argc > 2) {
		chidx = atoi(argv[2]);
		cout << "pulse height of ch " << chidx << endl;
	}

	aatop = A3_1;
	aabot = A3_2;
	doped = true;

	int firstframeidx;
	bool first = true;
	cout << "HI" << endl;
	initscr();
	timeout(0);
	int doIncrement = true;
	int doDraw = true;
	int disp = 0;
	while (1 == 1) {
		//rc1 = recvfrom(sd, &firstp, sizeof(firstp), 0, (struct sockaddr *) &clientaddr, (socklen_t*) (&clientaddrlen));

		usleep(1000);
		if(doIncrement){
			cFrames+=si;
		}else{
			usleep(100000);
		}
		firstp = sfl->readFrame(sfl->firstFrameNumber+sfl->offset+cFrames);
		// socket = ss->Accept();

		// if ((secondp.framenum%50)==0)  hchtot->Reset();
		if(doDraw){
			if (irun < 50)
				hchtot->Reset();
			his1->Reset();
			his2->Reset();
			his3->Reset();
			his4->Reset();

			for (i = 0; i < NCH; i++) {
				dsvalue[i] = float((firstp->inar[i]));

			}

			if (irun == 400) {
				filepede.open((const char *) ("current_pede.dat"), ios::out);
				for (int j = 0; j < NCH; j++)
					filepede << j << " " << int(runav[j] / 400.00) << endl;
				for (int j = 0; j < NCH; j++)
					//cout << j << " adc=" << ((int(runav[j]) >> 9) & 0xf) << " adc_ch=" << ((int(runav[j]) >> 5) & 0xf) << " chip ch=" << ((int(runav[j])) & 0x1f) << "     " << runav[j] << endl;
				filepede.close();
				hchtot->Reset();
				his45->Reset();
				//cout << "pedestal file written  " << endl;
			}
			peso = TMath::Min(50, irun);
			for (i = 0; i < NCH; i++) {

				if (irun < 400)
					runav[i] = (runav[i] + dsvalue[i]); //peso+dsvalue[i])*(1/(peso+1));
				his2->Fill(float(i), float(dsvalue[i] - (runav[i] / 400.0)));
				his3->Fill(float((i % 2) * 640 + int(i / 2)), float(dsvalue[i] - (runav[i] / 400.0)));
				his4->Fill(float(((i + 1) % 2) * 640 + int(i / 2)), float(dsvalue[i] - (runav[i] / 400.0)));
				his1->Fill(float(i), dsvalue[i]);
			}
			hchtot->Fill(float(dsvalue[chidx] - (runav[chidx] / 400.0)), 1.);
			his45->Fill(float(dsvalue[chidx] - (runav[chidx] / 400.0)), float(dsvalue[chidx + 1] - (runav[chidx + 1] / 400.0)), 1.);

			//if ((irun % 7) == 0) {
				//cout << "PLOTTING >> irun is " << irun << " frame num is " << (firstp->framenum - firstframeidx) / 2 << endl;
				ytime++;

				aatop->cd();
				his1->GetXaxis()->SetRangeUser(MIN_CH_SHOW, MAX_CH_SHOW);
				his1->GetYaxis()->SetRangeUser(3000, 8000);
				his1->DrawCopy();
				his2->GetXaxis()->SetRangeUser(MIN_CH_SHOW, MAX_CH_SHOW);
				his2->GetYaxis()->SetRangeUser(MIN_POS, MAX_POS);
				aabot->cd();
				his2->DrawCopy();
				A33->cd();

				A33->SetLogy(1);

				hchtot->SetLineColor(4);
				hchtot->DrawCopy("");

				hchtot->SetLineColor(2);
				hchtot->DrawCopy("");

				A3->Update();

				A2->cd();

				his45->DrawCopy("colz");
				A2->Update();
				A3->cd();
				A3->Modified();

			//}
			irun++;
		}


		if(!doIncrement){
			doDraw = false;
		}

		char s[100];
		s[0] = '\0';

		int c = getch();
		mvprintw(3,0,"CURRENT FRAME SHOULD BE:\t %i           ", cFrames);

#ifdef SMALL_FRAMES
		mvprintw(4,0,"CURRENT FRAME IS:\t\t%u                 ", firstp->framenum);
		mvprintw(5,0,"DIFFERENCE IS:\t\t%u                    ", sfl->firstFrameNumber+sfl->offset+cFrames-firstp->framenum);
		mvprintw(11,0,"Frames in Dataset: %u                  POS: %f %%                                       ",sfl->firstFrameNumber-sfl->lastFrameNumber,(double)(firstp->framenum-sfl->firstFrameNumber)/(double)(sfl->lastFrameNumber-sfl->firstFrameNumber)*100.0);
#else
		mvprintw(4,0,"CURRENT FRAME IS:\t\t%u                 ", firstp->frame1_number);
		mvprintw(5,0,"DIFFERENCE IS:\t\t%u                    ", sfl->firstFrameNumber+sfl->offset+cFrames-firstp->frame1_number);
		mvprintw(11,0,"Frames in Dataset: %u                  POS: %f %%                                       ",sfl->firstFrameNumber-sfl->lastFrameNumber,(double)(firstp->frame1_number-sfl->firstFrameNumber)/(double)(sfl->lastFrameNumber-sfl->firstFrameNumber)*100.0);
#endif

		mvprintw(12,0,"SVN: %s",SVN_REV);

		mvprintw(7,0,"s: INC n: next frame p: pre frame j: %i frwrd b: %i bkwrd r: reset pedestal q: quit",bi,bi);

		if(disp < 0){
			mvprintw(1,0,"                                   ");
			mvprintw(2,0,"                                   ");
		}

		if(irun < 400){
			mvprintw(9,0,"CALC PEDESTAL ... (%i)    ",400 - irun);
		}else{
			mvprintw(9,0,"CALC PEDESTAL ... DONE    ");
		}

		if(c>0){
			disp = 30;
			mvprintw(1,0,"THAT? %c \n",c);
			mvprintw(2,0,"                                                         ");
			if(c == 's'){
				doIncrement = !doIncrement;
				if(doIncrement){
					doDraw = true;
					mvprintw(2,0,"INCREMENT IS ON");
				}else{
					mvprintw(2,0,"INCREMENT IS OFF");
				}
			}else
			if(c == 'n'){
				cFrames+=si;
				mvprintw(2,0,"NEXT FRAME");
				doDraw = true;
			}else
			if(c == 'p'){
				cFrames-=si;
				doDraw = true;
				mvprintw(2,0,"PREV FRAME");
			}else
			if(c == 'j'){
				cFrames+=bii;
				doDraw = true;

				mvprintw(2,0,"JUMP %i FORWARD",bi);
			}else
			if(c == 'b'){
				cFrames-=bii;
				doDraw = true;
				mvprintw(2,0,"JUMP %i BACKWARD",bi);
			}else
			if(c == 'q'){
				mvprintw(2,0,"GO AWAY");
				break;
			}else

			if(c == 'r'){
				mvprintw(2,0,"RESET PEDESTAL");
				for (i = 0; i < NCH; i++)
					runav[i] = 0.0;
				irun = 0;
			}else{
				mvprintw(2,0,"NO Try AGAIN");
			}

		}else{
			mvprintw(0,0,"WHAT?");
		}

        if(disp % 1000 == 0){
           sfl->update();
        }

		refresh();


		disp--;
		free(firstp);

	}

	if (sd != -1)
		close(sd);



	//rootapp->Run();
	 endwin();
	return 0;

}

