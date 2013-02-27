#ifndef TREE_MANAGER_H
#define TREE_MANAGER_H

#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"

#include "frame.h"
#include "random_suffix.h"

namespace gotthard{

class TreeManager {
public:
    TreeManager(int max_frames_per_file,
            std::string output_prefix);
    void make_branches(int& frame_number, Frame& frame);
    std::string current_file_name() {return current_file_name_;}
    int Fill();
    void Close() {tree_.Write(); file_.Close();}

private:
    std::string output_prefix_;
    std::string current_file_name_;
    TFile* file_;
    TTree* tree_;

    //manage the files
    const int max_frames_per_file_;
    int frames_written_;
    int* frame_number_;
    Frame* frame_;
};
}
#endif /* end of include guard: TREE_MANAGER_H */
