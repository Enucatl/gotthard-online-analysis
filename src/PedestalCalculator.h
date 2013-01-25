#include <string>
#include <queue>
#include <cmath>

#include "TH1.h"
#include "TFile.h"

#include "gotthard_constants.h"
#include "FrameROOTFunctions.h"

class PedestalCalculator {
public:
    PedestalCalculator();
    int get_pedestal(TH1D& output_histogram);
    int get_pedestal(std::vector<double>& output_vector);
    int save_histogram(std::string output_name);
    bool empty() const { return full_frames_.empty(); }
    int size() const { return full_frames_.size(); }
    void push(const FullFrame& frame);
    void pop();

private:
    std::queue<FullFrame> full_frames_;
    TH1D pedestal_histogram_;
    //the temporary histogram is used to convert frames to TH1 before adding
    //them to the current pedestal
    TH1D temp_histogram_;
    //min value for a photon to be counted
    unsigned int threshold;
    const int min_frames_;
};
