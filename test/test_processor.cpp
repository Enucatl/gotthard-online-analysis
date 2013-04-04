#define BOOST_TEST_MODULE processor

#include <iostream>
#include <fstream>

#include <boost/test/included/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <boost/filesystem.hpp>

#include "constants.h"
#include "frame.h"
#include "frame_reader.h"
#include "pedestal_calculator.h"
#include "trigger.h"
#include "tree_manager.h"
#include "frame_processor.h"
#include "raw_image_tools.h"

namespace tt = boost::test_tools;
int n = gotthard::kNumberOfChannels;

BOOST_AUTO_TEST_SUITE(processor_test)

BOOST_AUTO_TEST_CASE(process_files) {
    typedef gotthard::FrameProcessor<
        Frame,
        gotthard::FrameReader,
        gotthard::PedestalCalculator,
        gotthard::trigger::ThresholdTrigger,
        gotthard::TreeManager> Processor;
    std::vector<fs::path> files;
    fs::path folder(".");
    gotthard::get_all_raw_files(folder, files);
    Processor processor(
            0, gotthard::kNumberOfChannels,
            10,
            20,
            100,
            "root_test_processor"
            );
    processor(files);
}

BOOST_AUTO_TEST_SUITE_END()
