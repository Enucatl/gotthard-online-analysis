#include "tree_manager.h"

namespace gotthard {

using gotthard::random::get_random_suffix;

TreeManager::TreeManager(int max_frames_per_file,
        std::string output_prefix): 
    output_prefix_(output_prefix),
    current_file_name_(output_prefix + get_random_suffix(3) + ".root"),
    file_(current_file_name_.c_str(), "create"),
    tree_("gotthard_tree", "gotthard_tree"),
    max_frames_per_file_(max_frames_per_file),
    frames_written_(0) {
}

void TreeManager::make_branches(int& frame_number, Frame& frame) {
    tree_.Branch("frame_number", &frame_number, "frame_number/I");
    tree_.Branch("frame", &frame);
}

int TreeManager::Fill() {
    if (frames_written_ < max_frames_per_file_) {
        tree_.Fill();
    }
    else {
        file_.Write();
        file_.Close();
        frames_written_ = 0;
    }
    frames_written_++;
    return frames_written_;
}
}
