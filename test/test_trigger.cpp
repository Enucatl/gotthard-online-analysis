#define BOOST_TEST_MODULE trigger_test

#include <iostream>
#include <fstream>

#include <boost/test/included/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include "constants.h"
#include "frame.h"
#include "frame_reader.h"
#include "pedestal_calculator.h"
#include "trigger.h"

namespace tt = boost::test_tools;
int n = gotthard::kNumberOfChannels;

BOOST_AUTO_TEST_SUITE(test_trigger)

BOOST_AUTO_TEST_CASE(check_trigger) {
    //read pedestal frame
    int frame_id = -999;
    Frame frame(n, 0);
    gotthard::FrameReader reader(0, n);
    std::string file_name("fake.raw");
    std::ifstream file(file_name.c_str());
    BOOST_REQUIRE(file.is_open());
    reader.read_next_frame(file, frame_id, frame);
    BOOST_CHECK(frame_id == 1);
    for (int i = 0; i < n; i++) {
        BOOST_CHECK(frame[i] == 100);
    }
    gotthard::PedestalCalculator pedestal_calculator(1, n);
    pedestal_calculator.push(frame);

    gotthard::trigger::ThresholdTrigger trigger(n, gotthard::trigger::kThresholdLow);
    reader.read_next_frame(file, frame_id, frame);
    BOOST_CHECK(frame_id == 3);
    bool triggered = trigger.subtract(frame, pedestal_calculator.get_pedestal());
    BOOST_CHECK(triggered);
}

BOOST_AUTO_TEST_CASE(check_trigger_swap) {
    //do the same things as in check_trigger
    int frame_id = -999;
    Frame frame(n, 0);
    gotthard::FrameReader reader(0, n);
    std::string file_name("fake.raw");
    std::ifstream file(file_name.c_str());
    BOOST_REQUIRE(file.is_open());
    reader.read_next_frame(file, frame_id, frame);
    gotthard::PedestalCalculator pedestal_calculator(1, n);
    pedestal_calculator.push(frame);
    gotthard::trigger::ThresholdTrigger trigger(n, gotthard::trigger::kThresholdLow);
    reader.read_next_frame(file, frame_id, frame);
    trigger.subtract(frame, pedestal_calculator.get_pedestal());
    trigger.swap_with_subtracted(frame);
    for (int i = 0; i < n; i++) {
        int value = 0;
        if (i == 2)
            value = 800;
        else
            value = 50;
        BOOST_CHECK(frame[i] == value);
        //std::cout << frame[i] << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END()
