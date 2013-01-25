CFLAGS=-Wall `root-config --cflags`
LDFLAGS=`root-config --glibs`

FrameLoader: FrameLoader* Packet* FullFrame*
	g++ -c ${CFLAGS} ${LDFLAGS} FrameLoader.cpp Packet.cpp FullFrame.cpp

Packet: Packet*
	g++ -c ${CFLAGS} ${LDFLAGS} Packet.cpp 

FullFrame: FullFrame*
	g++ -c ${CFLAGS} ${LDFLAGS} FullFrame.cpp 

PedestalCalculator: PedestalCalculator*
	g++ -c ${CFLAGS} ${LDFLAGS} PedestalCalculator.cpp 

FrameROOTFunctions: FrameROOTFunctions.h FrameROOTFunctions.cpp
	g++ -c ${CFLAGS} ${LDFLAGS} FrameROOTFunctions.cpp 

Spectrum: Spectrum.cpp Spectrum.h
	g++ -c ${CFLAGS} ${LDFLAGS} Spectrum.cpp 

OfflineTrigger: OfflineTrigger.cpp OfflineTrigger.h
	g++ -c ${CFLAGS} ${LDFLAGS} OfflineTrigger.cpp 
