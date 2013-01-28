CFLAGS=-Wall `root-config --cflags`
LDFLAGS=`root-config --glibs`
BOOST_LIBS=-lboost_program_options -lboost_system -lboost_filesystem

FrameLoader: FrameLoader.* Packet.* FullFrame.*
	g++ -c FrameLoader.cpp Packet.cpp FullFrame.cpp $(CFLAGS) $(LDFLAGS) 

Packet: Packet.cpp Packet.h
	g++ -c Packet.cpp $(CFLAGS) $(LDFLAGS) 

FullFrame: FullFrame.cpp FullFrame.h Packet
	g++ -c FullFrame.cpp $(CFLAGS) $(LDFLAGS) 

PedestalCalculator: PedestalCalculator.cpp PedestalCalculator.h
	g++ -c PedestalCalculator.cpp $(CFLAGS) $(LDFLAGS) 

FrameROOTFunctions: FrameROOTFunctions.h FrameROOTFunctions.cpp
	g++ -c FrameROOTFunctions.cpp $(CFLAGS) $(LDFLAGS) 

Spectrum: Spectrum.cpp Spectrum.h
	g++ -c Spectrum.cpp $(CFLAGS) $(LDFLAGS) 

OfflineTrigger: OfflineTrigger.cpp OfflineTrigger.h
	g++ -c OfflineTrigger.cpp $(CFLAGS) $(LDFLAGS)

gotthard_utils: gotthard_utils.*
	g++ -c gotthard_utils.cpp $(BOOST_LIBS) $(CFLAGS) $(LDFLAGS) 

OfflineTriggerStandalone: OfflineTrigger FrameROOTFunctions PedestalCalculator FullFrame Packet gotthard_utils
	g++ -o offline_trigger offline_trigger.cpp OfflineTrigger.cpp FrameROOTFunctions.cpp PedestalCalculator.cpp FullFrame.cpp Packet.cpp gotthard_utils.cpp $(CFLAGS) $(BOOST_LIBS) $(LDFLAGS) 

clean:
	rm *.so *.d *.o *_.cxx
