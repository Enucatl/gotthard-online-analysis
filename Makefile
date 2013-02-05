.PHONY: clean all
.SUFFIXES: .cpp .o

SRC_FOLDER=src
INC_FOLDER=include
LIB_FOLDER=lib
TEST_FOLDER=test

CFLAGS=-Wall -I$(INC_FOLDER)

vpath %.cpp $(SRC_FOLDER) $(TEST_FOLDER)
vpath %.h $(INC_FOLDER)

all:
	echo "nothing"

tests: test_frame_reader

test_frame_reader: test_frame_reader.cpp $(addprefix $(LIB_FOLDER)/, frame_reader.o)
	g++ $(CFLAGS) -o $(TEST_FOLDER)/$@ $^

$(LIB_FOLDER)/%.o: %.cpp %.h
	g++ -c $(CFLAGS) -o $@ $< 

clean:
	-rm lib/*.*o python/*.pyc online_viewer single_image_reader
