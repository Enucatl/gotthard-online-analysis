#define BOOST_TEST_MODULE root_manager_test

#include <iostream>
#include <string>
#include <fstream>

#include <boost/test/included/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include "constants.h"
#include "frame.h"
#include "frame_reader.h"
#include "tree_manager.h"

namespace tt = boost::test_tools;
int n = gotthard::kNumberOfChannels;

BOOST_AUTO_TEST_SUITE(root_manager_test)

BOOST_AUTO_TEST_CASE(write_one_frame) {
    int frame_id = -999;
    Frame frame(n, 0);
    gotthard::FrameReader reader(0, n);
    std::string output_prefix = "root_fake_tree";

    int max_frames_per_file = 2;
    gotthard::TreeManager tree_manager(max_frames_per_file, output_prefix);
    tree_manager.make_branches(frame_id, frame);

    std::string file_name("fake.raw");
    std::ifstream file(file_name.c_str());
    reader.read_next_frame(file, frame_id, frame);
    tree_manager.Fill();
    tree_manager.Close();
    //reopen root_file
    TFile root_file(tree_manager.current_file_name().c_str());
    BOOST_REQUIRE(root_file.IsOpen());
    TTree* tree = dynamic_cast<TTree*>(root_file.Get("gotthard_tree"));
    BOOST_REQUIRE(tree);

    Frame* read_frame = 0;
    int read_frame_id;
    tree->SetBranchAddress("frame_number", &read_frame_id);
    tree->SetBranchAddress("frame", &read_frame);
    tree->GetEntry(0);
    BOOST_CHECK(read_frame_id == 1);
    for (int i = 0; i < n; i++) {
        BOOST_CHECK(read_frame->at(i) == 100);
    }
}

BOOST_AUTO_TEST_SUITE_END()
