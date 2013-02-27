#define BOOST_TEST_MODULE trigger_and_pedestal_test

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

BOOST_AUTO_TEST_SUITE(trigger_and_pedestal_test)

BOOST_AUTO_TEST_CASE(check_passing_frame) {
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

    gotthard::trigger::Trigger trigger(n, gotthard::trigger::kThresholdLow);

    reader.read_next_frame(file, frame_id, frame);
    trigger(frame, pedestal_calculator.get_pedestal());
    BOOST_CHECK(frame_id == 3);
    pedestal_calculator.push(frame);
    BOOST_CHECK(frame[2] == 100);
    for (int i = 0; i < n; i++) {
        double value = pedestal_calculator.get_pedestal()[i];
        if (i != 2)
            BOOST_CHECK(value == 150);
        else
            BOOST_CHECK(value == 100);
    }
    trigger.swap_with_subtracted(frame);
    //frame is now subtracted
    for (int i = 0; i < n; i++) {
        if (i != 2)
            BOOST_CHECK(frame[i] == 50);
        else
            BOOST_CHECK(frame[i] == 800);
    }
}

BOOST_AUTO_TEST_SUITE_END()
