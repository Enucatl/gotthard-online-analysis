
CONFIG=Nothing	#can be overitten by CONFIG=APS for example

SVNDEV = -D'SVN_REV="$(shell svnversion -n .)"'

BIN_DIR=./bin
CPP_DIR=./src
H_DIR=./src

UI_SRC=IISDict.cpp InterpolationMainFrame.cpp GotthardConfigurator.cpp InterpolationMainModel.cpp

SRC=$(CPP_DIR)/FrameLoader.cpp $(CPP_DIR)/EtaCreator.cpp $(CPP_DIR)/StreamOperators.cpp $(CPP_DIR)/SmallFrameLoader.cpp
INCLUDES=$(H_DIR)/InterpolationMainFrame.h $(H_DIR)/InterpolationMainModel.h $(H_DIR)/GotthardConfigurator.h $(H_DIR)/SmallFrameLoader.h


MAINDSINFO=$(CPP_DIR)/DSInfo.cpp

MAINETACREATOR=$(CPP_DIR)/EtaCreatorMain.cpp

MAINSINOGRAM=$(CPP_DIR)/CreateSinogram.cpp

MAINOEASYDISP=$(CPP_DIR)/offlineEasydisp.cpp

all: dsInfo etaCreatorBig etaCreatorSmall analyzeScanSmall analyzeScanBig offlineEasydispSmall offlineEasydispBig

dsInfo: $(BIN_DIR)/dsInfo
etaCreatorSmall: $(BIN_DIR)/etaCreatorSmall
etaCreatorBig: $(BIN_DIR)/etaCreatorBig
analyzeScanSmall: $(BIN_DIR)/analyzeScanSmall
analyzeScanBig: $(BIN_DIR)/analyzeScanBig
offlineEasydispSmall: $(BIN_DIR)/offlineEasydispSmall
offlineEasydispBig: $(BIN_DIR)/offlineEasydispBig

$(BIN_DIR)/dsInfo: $(SRC) $(MAINDSINFO)
	g++ -DMYROOT `root-config --cflags --glibs` -o $(BIN_DIR)/dsInfo $(SRC)  $(MAINDSINFO) $(SVNDEV)

$(BIN_DIR)/etaCreatorSmall: $(SRC) $(MAINETACREATOR)
	g++ -DMYROOT -g `root-config --cflags --glibs` -o $(BIN_DIR)/etaCreatorSF $(SRC)  $(MAINETACREATOR) -DSMALL_FRAMES -D$(CONFIG) $(SVNDEV)

$(BIN_DIR)/etaCreatorBig: $(SRC) $(MAINETACREATOR)
	g++ -DMYROOT `root-config --cflags --glibs` -o $(BIN_DIR)/etaCreatorBF $(SRC)  $(MAINETACREATOR) -D$(CONFIG) $(SVNDEV)

$(BIN_DIR)/analyzeScanSmall: $(SRC) $(MAINSINOGRAM)
	g++ -DMYROOT -g `root-config --cflags --glibs` -o $(BIN_DIR)/analyzeScanSF $(SRC)  $(MAINSINOGRAM) -DSMALL_FRAMES -D$(CONFIG) $(SVNDEV)

$(BIN_DIR)/analyzeScanBig: $(SRC) $(MAINSINOGRAM)
	g++ -DMYROOT -g `root-config --cflags --glibs` -o $(BIN_DIR)/analyzeScanBF $(SRC)  $(MAINSINOGRAM) -D$(CONFIG) $(SVNDEV)


$(BIN_DIR)/offlineEasydispSmall: $(MAINOEASYDISP) $(SRC)
	g++ -DMYROOT -g `root-config --cflags --glibs` -o $(BIN_DIR)/offlineEasydispSF $(SRC)  $(MAINOEASYDISP) -l ncurses -DSMALL_FRAMES $(SVNDEV)

$(BIN_DIR)/offlineEasydispBig: $(MAINOEASYDISP) $(SRC)
	g++ -DMYROOT -g `root-config --cflags --glibs` -o $(BIN_DIR)/offlineEasydispBF $(SRC)  $(MAINOEASYDISP) -l ncurses $(SVNDEV)

clean:
	rm -f $(BIN_DIR)/dsInfo $(BIN_DIR)/etaCreatorSF $(BIN_DIR)/etaCreatorBF $(BIN_DIR)/analyzeScanSF $(BIN_DIR)/analyzeScanBF $(BIN_DIR)/offlineEasydispSF $(BIN_DIR)/offlineEasydispBF

