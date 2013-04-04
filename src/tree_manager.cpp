#include "tree_manager.h"

namespace gotthard {

using gotthard::random::get_random_suffix;

TreeManager::TreeManager(int max_frames_per_file,
        std::string output_prefix,
        int* frame_number,
        Frame* frame): 
    output_prefix_(output_prefix),
    current_file_name_(output_prefix + get_random_suffix(3) + ".root"),
    file_(new TFile(current_file_name_.c_str(), "create")),
    tree_(new TTree("gotthard_tree", "gotthard_tree")),
    max_frames_per_file_(max_frames_per_file),
    frames_written_(0),
    frame_number_(frame_number),
    frame_(frame),
    closed_(false) {
        make_branches();
}

TreeManager::~TreeManager() {
    //the TTree is automatically deleted by the file, as explained by
    //http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=16123&p=69212#p69212
    if (not closed_)
        Close();
}

void TreeManager::make_branches() {
    tree_->Branch("frame_number", frame_number_, "frame_number/I");
    tree_->Branch("frame", frame_);
}

int TreeManager::Fill() {
    if (frames_written_ >= max_frames_per_file_) {
        Close();
        frames_written_ = 0;
        Open();
    }
    frames_written_++;
    return tree_->Fill();
}

void TreeManager::Close() {
    closed_ = true;
    file_->Write();
    file_->Close();
    //the TTree is automatically deleted by the file, as explained by
    //http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=16123&p=69212#p69212
    delete file_;
}

void TreeManager::Open() {
    current_file_name_ = output_prefix_ + get_random_suffix(3) + ".root";
    file_ = new TFile(current_file_name_.c_str(), "create");
    tree_ = new TTree("gotthard_tree", "gotthard_tree");
    make_branches();
    closed_ = false;
}

}
