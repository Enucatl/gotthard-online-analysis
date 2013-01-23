#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#include <boost/progress.hpp>

#include "TH1.h"
#include "TH2.h"
#include "TSpectrum.h"

#include "Packet.h"
#include "FullFrame.h"
#include "FrameROOTFunctions.h"
#include "FrameLoader.h"
#include "PedestalCalculator.h"
#include "gotthard_constants.h"

class Spectrum {
public:
    Spectrum(std::string filePrefix, int trialNumber,
            int max_photons_per_frame,
            int number_of_bins, int min_bin, int max_bin);

    int read_frames(int amount_of_frames);
    void set_queue_size(int size) { queue_max_size_ = size; }
    int get_threshold() const { return threshold_; }
    void set_threshold(int value) { threshold_ = value; }
    void set_max_photons_per_frame(int value) { max_photons_per_frame_ = value; }
    TH1D get_spectrum() const { return spectrum_; }
    std::vector<TH1D> get_single_pixel_spectrum() const { return single_pixel_spectrum_; }
    TH2D get_near_pixel_correlation() const { return near_pixel_correlation_; }

private:
    int queue_max_size_;
    int max_photons_per_frame_;
    int threshold_;

    FrameLoader frame_loader_;

    TH1D spectrum_;
    std::vector<TH1D> single_pixel_spectrum_;
    TH2D near_pixel_correlation_;
    TH1D frame_hist_;
    TH1D pedestal_;
    TSpectrum peak_finder_;
    FullFrame frame_;

    PedestalCalculator pedestal_calculator_;
};

#endif /* end of include guard: SPECTRUM_H */
