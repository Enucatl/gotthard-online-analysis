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

bool PedestalCalculator::push(const FullFrame& frame){
    //returns true if a photon was found
    //useful for triggering purposes
    int n_frames = full_frames_.size();
    full_frames_.push(frame);
    bool photon_found = false;
    if (n_frames > min_frames_) {
        double factor = 1.0 / n_frames;
        for (int i = gotthard_constants::kMinUsefulChannel; i < gotthard_constants::kMaxUsefulChannel; i++) {
            double value = frame.get_pixel(i);
            double old_value = pedestal_histogram_.GetBinContent(i + 1);
            double average = old_value * factor;
            //std::cout << "value: " << value << " " << "average: " << average << std::endl;
            //skip strips where a photon was detected
            if ((value - average) > gotthard_constants::kNoiseThreshold) {
                value = average;
                photon_found = true;
            }
            //std::cout << value << " " << old_value << " " << average  << std::endl;
            pedestal_histogram_.SetBinContent(i + 1, old_value + value);
        }
    }
    else {
        frame_root_functions::get_histogram(frame, temp_histogram_);
        pedestal_histogram_.Add(&temp_histogram_);
    }
    return photon_found;
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

int PedestalCalculator::get_pedestal(std::vector<double>& output_vector) {
    //if (static_cast<int>(output_vector.size()) != gotthard_constants::kNumberOfChannels) {
        //output_vector.resize(gotthard_constants::kNumberOfChannels, 0);
    //}
    double factor = 1.0 / full_frames_.size();
    for (int i = 0; i < gotthard_constants::kNumberOfChannels; i++) {
        output_vector[i] = pedestal_histogram_.GetBinContent(i + 1) * factor;
    }
    return 0;
}

int PedestalCalculator::save_histogram(std::string output_name){
    TFile output_file(output_name.c_str(), "recreate");
    output_file.cd();
    return pedestal_histogram_.Write();
}
