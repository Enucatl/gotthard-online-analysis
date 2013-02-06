#define BOOST_TEST_MODULE frame_reader_test

#include <iostream>
#include <fstream>

#include <boost/test/included/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include "constants.h"

#include "frame_reader.h"

namespace tt = boost::test_tools;
int n = gotthard::kNumberOfChannels;

BOOST_AUTO_TEST_SUITE(pedestal_calculator_test)

BOOST_AUTO_TEST_CASE(check_pedestal_values) {
    int frame_id = -999;
    std::vector<double> frame(n, 0);
    gotthard::FrameReader reader(0, n);
    std::string file_name("fake.raw");
    std::ifstream file(file_name.c_str());
    reader.read_next_frame(file, frame_id, frame);
    gotthard::PedestalCalculator pedestal_calculator(1, n);
    pedestal_calculator.push(frame);
    std::vector<double> pedestal_frame = pedestal_calculator.get_pedestal();
    for (int i = 0; i < n; i++) {
        BOOST_CHECK(pedestal_frame[i] == 100);
    }
}

BOOST_AUTO_TEST_CASE(check_pedestal_push_pop) {
    int frame_id = -999;
    std::vector<double> frame(n, 0);
    gotthard::FrameReader reader(0, n);
    std::string file_name("fake.raw");
    std::ifstream file(file_name.c_str());
    reader.read_next_frame(file, frame_id, frame);
    gotthard::PedestalCalculator pedestal_calculator;
    BOOST_CHECK(pedestal_calculator.size() == 0);
    pedestal_calculator.push(frame);
    BOOST_CHECK(pedestal_calculator.size() == 1);
    pedestal_calculator.push(frame);
    BOOST_CHECK(pedestal_calculator.size() == 2);
    pedestal_calculator.pop(frame);
    BOOST_CHECK(pedestal_calculator.size() == 1);
}

BOOST_AUTO_TEST_SUITE_END()
