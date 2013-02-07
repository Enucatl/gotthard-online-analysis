.PHONY: clean all tests
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

tests: $(addprefix $(TEST_FOLDER)/, test_frame_reader test_pedestal_calculator write_fake_file test_bind)

$(TEST_FOLDER)/test_frame_reader: test_frame_reader.cpp $(addprefix $(LIB_FOLDER)/, frame_reader.o)
	g++ $(CFLAGS) -o $@ $^

$(TEST_FOLDER)/test_pedestal_calculator: test_pedestal_calculator.cpp $(addprefix $(LIB_FOLDER)/, frame_reader.o pedestal_calculator.o)
	g++ $(CFLAGS) -o $@ $^

$(TEST_FOLDER)/test_bind: test_bind.cpp 
	g++ $(CFLAGS) -o $@ $^

$(TEST_FOLDER)/write_fake_file: write_fake_file.cpp 
	g++ $(CFLAGS) -o $@ $^

$(LIB_FOLDER)/%.o: %.cpp %.h
	g++ -c $(CFLAGS) -o $@ $< 

clean:
	-rm lib/*.*o python/*.pyc online_viewer single_image_reader
