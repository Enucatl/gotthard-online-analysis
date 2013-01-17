#ifndef FRAMEROOTFUNCTIONS_H
#define FRAMEROOTFUNCTIONS_H

#include <vector>
#include <sstream>

#include "TH1.h"
#include "TTree.h"
#include "TMath.h"
#include "FullFrame.h"

namespace frame_root_functions {

int get_histogram(const FullFrame& frame, TH1D& histogram);
int get_average(const std::vector<FullFrame>& frames, TH1D& histogram);
int get_histogram_subtracting_pedestal(const FullFrame& frame, TH1D& pedestal, TH1& histogram);
void get_photon_tree(const std::vector<TH1D>& histograms, TTree& tree);
}

#endif /* end of include guard: FRAMEROOTFUNCTIONS_H */
