#define BOOST_TEST_MODULE frame_reader_test

#include <iostream>
#include <fstream>

#include <boost/test/included/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include "constants.h"

#include "frame_reader.h"

namespace tt = boost::test_tools;
int n = gotthard::kNumberOfChannels;

BOOST_AUTO_TEST_SUITE(frame_reader_test)

BOOST_AUTO_TEST_CASE(check_read_frame) {
    int frame_id = -999;
    std::vector<double> frame(n, 0);
    gotthard::FrameReader reader(0, n);
    std::string file_name("single_frame.raw");
    std::ifstream file(file_name.c_str());
    reader.read_next_frame(file, frame_id, frame);
    //sanity check for length:
    int size = frame.size();
    BOOST_REQUIRE(size == n);
    //match against 'golden' log file
    //with frame number and all the strip values
    tt::output_test_stream output("single_frame_golden.log", true);
    output << frame_id << std::endl;
    std::cout << frame_id << std::endl;
    for (int i = 0; i < n; i++) {
        output << frame[i] << " ";
        std::cout << frame[i] << " ";
    }
    BOOST_CHECK(output.match_pattern());
}

BOOST_AUTO_TEST_CASE(check_fake_frames) {
    int frame_id = -999;
    std::vector<double> frame(n, 0);
    gotthard::FrameReader reader(0, n);
    std::string file_name("fake.raw");
    std::ifstream file(file_name.c_str());
    reader.read_next_frame(file, frame_id, frame);
    //sanity check for length:
    int size = frame.size();
    BOOST_REQUIRE(size == n);
    //first frame (pedestal) with all values set to 100
    BOOST_CHECK(frame_id == 1);
    for (int i = 0; i < n; i++) {
        BOOST_CHECK(frame[i] == 100);
    }
    //read second frame (with 150 everywhere, except for 900 at strip 2)
    reader.read_next_frame(file, frame_id, frame);
    //sanity check for length:
    size = frame.size();
    BOOST_REQUIRE(size == n);
    BOOST_CHECK(frame_id == 3);
    for (int i = 0; i < n; i++) {
        if (i != 2)
            BOOST_CHECK(frame[i] == 150);
        else 
            BOOST_CHECK(frame[i] == 900);
    }
}

BOOST_AUTO_TEST_SUITE_END()
