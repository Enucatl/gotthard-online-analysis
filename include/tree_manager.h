#ifndef TREE_MANAGER_H
#define TREE_MANAGER_H

#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"

#include "frame.h"
#include "random_generator.h"

namespace gotthard{

class TreeManager {

public:
    TreeManager(int max_frames_per_file,
            std::string output_prefix,
            int* frame_number,
            Frame* frame);
    virtual ~TreeManager();

    //return the name of the file where we are currently writing
    std::string current_file_name() {return current_file_name_;}

    //returns bytes written, -1 if write error:
    //http://root.cern.ch/root/html/TTree.html#TTree:Fill
    int Fill();

    //close and open TFile file_
    void Close();
    void Open();

private:
    //Make the branches for the tree, and set their address to frame_number_
    //and frame_
    void make_branches();

    //The prefix for all the output ROOT files
    std::string output_prefix_;
    std::string current_file_name_;
    TFile* file_;
    TTree* tree_;

    const int max_frames_per_file_;
    int frames_written_;
    int* frame_number_;
    Frame* frame_;

    //Was the pointer to the file deleted?
    bool closed_;
};
}
#endif /* end of include guard: TREE_MANAGER_H */
