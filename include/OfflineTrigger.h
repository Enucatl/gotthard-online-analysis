#ifndef OFFLINETRIGGER_H
#define OFFLINETRIGGER_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <boost/lexical_cast.hpp>

#include "TFile.h"
#include "TTree.h"

#include "Packet.h"
#include "FullFrame.h"
#include "PedestalCalculator.h"
#include "gotthard_constants.h"
#include "gotthard_utils.h"

class OfflineTrigger {
public:
    OfflineTrigger(std::string prefix, int run_number,
            int save_every, int pedestal_min_size, bool delete_files);
    ~OfflineTrigger();
    int start_triggering();

private:
    const std::string prefix_;
    const std::string extension_;
    const int run_number_;
    //save some empty frames for pedestal evaluation:
    //save one frame every save_every_ regardless of its photon content
    const int save_every_;
    const int pedestal_min_size_;
    const bool delete_files_;
    TFile file_;
    TTree tree_;
    FullFrame frame_;
    PedestalCalculator pedestal_calculator_;
    std::vector<double> current_pedestal_;
    int current_frame_number_;
};
#endif /* end of include guard: OFFLINETRIGGER_H */
