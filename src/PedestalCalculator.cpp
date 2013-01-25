#include "PedestalCalculator.h"

PedestalCalculator::PedestalCalculator():
    pedestal_histogram_("pedestal", "pedestal",
            gotthard_constants::kNumberOfChannels,
            0,
            gotthard_constants::kNumberOfChannels),
    temp_histogram_("temp", "temp",
            gotthard_constants::kNumberOfChannels,
            0,
            gotthard_constants::kNumberOfChannels),
    min_frames_(100) {
    }

void PedestalCalculator::push(const FullFrame& frame){
    int n_frames = full_frames_.size();
    full_frames_.push(frame);
    if (n_frames > min_frames_) {
        double factor = 1.0 / n_frames;
        for (int i = 0; i < gotthard_constants::kNumberOfChannels; i++) {
            double value = frame.get_pixel(i);
            double old_value = pedestal_histogram_.GetBinContent(i + 1);
            double average = old_value * factor;
            //skip strips where a photon was detected
            if (fabs(average - value) > gotthard_constants::kNoiseThreshold)
                value = average;
            pedestal_histogram_.SetBinContent(i + 1, old_value + value);
        }
    }
    else {
        frame_root_functions::get_histogram(frame, temp_histogram_);
        pedestal_histogram_.Add(&temp_histogram_);
    }
}

void PedestalCalculator::pop(){
    frame_root_functions::get_histogram(full_frames_.front(), temp_histogram_);
    pedestal_histogram_.Add(&temp_histogram_, -1);
    full_frames_.pop();
}

int PedestalCalculator::get_pedestal(TH1D& output_histogram) {
    if (output_histogram.GetNbinsX() != gotthard_constants::kNumberOfChannels) {
        return -1;
    }
    double factor = 1.0 / full_frames_.size();
    for (int i = 0; i < gotthard_constants::kNumberOfChannels; i++) {
        output_histogram.SetBinContent(i + 1, pedestal_histogram_.GetBinContent(i + 1) * factor);
    }
    return 0;
}

int PedestalCalculator::get_pedestal(FullFrame& output_frame) {
    if (output_frame.size() != gotthard_constants::kNumberOfChannels) {
        return -1;
    }
    double factor = 1.0 / full_frames_.size();
    for (int i = 0; i < gotthard_constants::kNumberOfChannels; i++) {
        output_frame.set_pixel(i, pedestal_histogram_.GetBinContent(i + 1) * factor);
    }
    return 0;
}

int PedestalCalculator::save_histogram(std::string output_name){
    TFile output_file(output_name.c_str(), "recreate");
    output_file.cd();
    return pedestal_histogram_.Write();
}
