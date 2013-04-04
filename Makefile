.PHONY: clean all tests compile_tests
.SUFFIXES: .cpp .o

SRC_FOLDER=src
INC_FOLDER=include
LIB_FOLDER=lib
BIN_FOLDER=bin
TEST_FOLDER=test
FOLDERS=$(BIN_FOLDER) $(LIB_FOLDER)

CFLAGS=-g -Wall -I$(INC_FOLDER) `root-config --cflags`
ROOT_LDFLAGS=`root-config --glibs`
BOOST_LDFLAGS=-lboost_random
BOOST_LDFLAGS_FILESYSTEM=-lboost_filesystem -lboost_system

vpath %.cpp $(SRC_FOLDER) $(TEST_FOLDER)
vpath %.h $(INC_FOLDER)

all:
	echo "nothing"

TESTS=test_frame_reader test_pedestal_calculator write_fake_file test_trigger test_trigger_and_pedestal test_tree_manager test_processor
compile_tests: $(addprefix $(TEST_FOLDER)/, $(TESTS))

tests: $(addprefix $(TEST_FOLDER)/, $(TESTS))
	echo "now running all tests!"
	cd test; for t in $(TESTS);\
	do echo \\n\\n\\n./$$t;\
	./$$t;\
	done
	rm -f root_*.root
	rm -f test/root_*.root

$(TEST_FOLDER)/test_processor: test_processor.cpp $(addprefix $(LIB_FOLDER)/, pedestal_calculator.o trigger.o raw_image_tools.o\
	frame_reader.o random_suffix.o tree_manager.o frame_processor.o)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LDFLAGS) $(BOOST_LDFLAGS) $(BOOST_LDFLAGS_FILESYSTEM)

$(TEST_FOLDER)/test_tree_manager: test_tree_manager.cpp $(addprefix $(LIB_FOLDER)/, frame_reader.o random_suffix.o tree_manager.o)
	g++ $(CFLAGS) -o $@ $^ $(ROOT_LDFLAGS) $(BOOST_LDFLAGS) 

$(TEST_FOLDER)/test_frame_reader: test_frame_reader.cpp $(addprefix $(LIB_FOLDER)/, frame_reader.o)
	g++ $(CFLAGS) -o $@ $^

$(TEST_FOLDER)/test_pedestal_calculator: test_pedestal_calculator.cpp $(addprefix $(LIB_FOLDER)/, frame_reader.o pedestal_calculator.o)
	g++ $(CFLAGS) -o $@ $^

$(TEST_FOLDER)/test_trigger: test_trigger.cpp $(addprefix $(LIB_FOLDER)/, frame_reader.o pedestal_calculator.o trigger.o)
	g++ $(CFLAGS) -o $@ $^

$(TEST_FOLDER)/test_trigger_and_pedestal: test_trigger_and_pedestal.cpp $(addprefix $(LIB_FOLDER)/, frame_reader.o pedestal_calculator.o trigger.o)
	g++ $(CFLAGS) -o $@ $^

$(TEST_FOLDER)/write_fake_file: write_fake_file.cpp 
	g++ $(CFLAGS) -o $@ $^

$(LIB_FOLDER)/%.o: %.cpp %.h | $(LIB_FOLDER)
	g++ -c $(CFLAGS) -o $@ $< 

$(FOLDERS): 
	mkdir -p $@

clean:
	-rm -r lib python/*.pyc 
