CFLAGS=-Wall `root-config --cflags`
LDFLAGS=`root-config --glibs`

FrameLoader: FrameLoader*
	g++ -c ${CFLAGS} ${LDFLAGS} FrameLoader.cpp 

Packet: Packet*
	g++ -c ${CFLAGS} ${LDFLAGS} Packet.cpp 

FullFrame: FullFrame*
	g++ -c ${CFLAGS} ${LDFLAGS} FullFrame.cpp 
