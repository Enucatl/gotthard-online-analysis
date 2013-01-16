#include "PedestalCalculator.h"

PedestalCalculator::PedestalCalculator(std::string file_prefix,
        int trial_number,
        int start_frame_number,
        int amount_of_full_frames):
frame_loader_(file_prefix, trial_number),
pedestal_histogram_("pedestal", "pedestal",
        gotthard_constants::kNumberOfChannels,
        0,
        gotthard_constants::kNumberOfChannels) {
    frame_loader_.readFrames(start_frame_number, amount_of_full_frames, full_frames_);
}

PedestalCalculator::~PedestalCalculator() {}

void PedestalCalculator::calculate_pedestal(){
    std::vector<double> total_frame(gotthard_constants::kNumberOfChannels, 0);
    for (std::vector<FullFrame>::iterator i = full_frames_.begin(); i != full_frames_.end(); ++i) {
        int total_frames = full_frames_.size();
        for (int j = 0; j < gotthard_constants::kNumberOfChannels; j++) {
            total_frame[j] += static_cast<double>(i->get_pixel(j)) / total_frames;
        }
    }
    for (int i = 0; i < gotthard_constants::kNumberOfChannels; i++) {
        pedestal_histogram_.SetBinContent(i + 1, total_frame[i]);
    }
}

int PedestalCalculator::save_histogram(std::string output_name){
    TFile output_file(output_name.c_str(), "recreate");
    output_file.cd();
    return pedestal_histogram_.Write();
}
