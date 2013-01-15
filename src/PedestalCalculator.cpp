#include "PedestalCalculator.h"

PedestalCalculator::PedestalCalculator(std::string file_prefix,
        int trial_number,
        int start_frame_number,
        int amount_of_full_frames):
frame_loader_(file_prefix, trial_number),
pedestal_histogram_("pedestal", "pedestal",
        CHANNEL_NUMBER, 0, CHANNEL_NUMBER) {
    full_frames_ = frame_loader_.readFrames(frame_loader_.firstFrameNumber() + start_frame_number, amount_of_full_frames);
}

PedestalCalculator::~PedestalCalculator(){
    for (std::vector<FullFrame*>::iterator i = full_frames_.begin(); i != full_frames_.end(); ++i) {
        delete *i;
    }
}

int PedestalCalculator::calculate_pedestal(){
    std::vector<double> total_frame(CHANNEL_NUMBER, 0);
    for (std::vector<FullFrame*>::iterator i = full_frames_.begin(); i != full_frames_.end(); ++i) {
        int total_frames = full_frames_.size();
        for (int j = 0; j < CHANNEL_NUMBER; j++) {
            total_frame[j] += static_cast<double>((*i)->inar[j]) / total_frames;
        }
    }
    for (int i = 0; i < CHANNEL_NUMBER; i++) {
        pedestal_histogram_.SetBinContent(i + 1, total_frame[i]);
    }
}

int PedestalCalculator::save_histogram(std::string output_name){
    TFile output_file(output_name.c_str(), "recreate");
    output_file.cd();
    return pedestal_histogram_.Write();
}
