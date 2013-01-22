#include "Spectrum.h"

Spectrum::Spectrum(std::string filePrefix, int trialNumber, 
        int max_photons_per_frame,
        int number_of_bins, int min_bin, int max_bin):
    max_photons_per_frame_(max_photons_per_frame), 
    frame_loader_(filePrefix, trialNumber),
    spectrum_("spectrum", "spectrum", number_of_bins, min_bin, max_bin),
    frame_hist_("frame_hist_", "frame_hist_",
            gotthard_constants::kNumberOfChannels, 0,
            gotthard_constants::kNumberOfChannels),
    pedestal_("pedestal_", "pedestal_",
            gotthard_constants::kNumberOfChannels, 0,
            gotthard_constants::kNumberOfChannels),
    peak_finder_(max_photons_per_frame_) {
        queue_max_size_ = 0;
        threshold_ = 200;
        single_pixel_spectrum_.reserve(gotthard_constants::kNumberOfChannels);
        for (int i = 0; i < gotthard_constants::kNumberOfChannels; i++) {
            std::stringstream name;
            name << "hist_pixel_" << i;
            single_pixel_spectrum_.push_back(
                    TH1D(name.str().c_str(),
                        name.str().c_str(),
                        number_of_bins, min_bin, max_bin));
        }
    }

int Spectrum::read_frames(int amount_of_frames){
    boost::progress_display show_progress(amount_of_frames);
    if (!queue_max_size_) {
        std::cerr << "WARNING! not subtracting pedestal" << std::endl;
        std::cerr << "queue_max_size_ = 0" << std::endl;
        return -1;
    }
    std::ifstream file;
    int file_id = 0;
    bool open_success = frame_loader_.openFile(file_id, file);
    for (int i = 0; i < amount_of_frames; i++) {
        if (not open_success) {
            file_id++;
            file.close();
            open_success = frame_loader_.openFile(file_id, file);
            break;
        }
        Packet empty_packet;
        frame_loader_.readNextCompleteFrame(file, frame_, empty_packet);
        if (frame_.frame1_number() < 0) {
            std::cout << "reached EOF" << std::endl;
            //reached end of current file
            file_id++;
            file.close();
            open_success = frame_loader_.openFile(file_id, file);
            continue;
        }
        pedestal_calculator_.push(frame_);
        ++show_progress;
        if (pedestal_calculator_.size() < queue_max_size_) {
            //don't analyze frames before the pedestal queue is filled up
            continue;
        }
        else {
            pedestal_calculator_.pop();
            pedestal_calculator_.get_pedestal(pedestal_);
            frame_root_functions::get_histogram_subtracting_pedestal(frame_, pedestal_, frame_hist_);
            //get maximum without the TH1::GetMaximum method since the following works even
            //if the y-range has been changed with TH1::SetMaximum
            double maximum = frame_hist_.GetBinContent(
                    frame_hist_.GetMaximumBin());
            if (maximum < threshold_) {
                //no photon counted
            }
            else { // a photon was found with value > threshold_
                int number_of_peaks = peak_finder_.Search(&frame_hist_, 2, "nodraw", 0.3);
                float* positions = peak_finder_.GetPositionX();
                frame_root_functions::subtract_common_mode(frame_hist_,
                        positions,
                        number_of_peaks);
                for (int j = 0; j < number_of_peaks; j++) {
                    int pixel = 1 + static_cast<int>(positions[j] + 0.5);
                    double energy = frame_hist_.GetBinContent(pixel);
                    //collect nearby pixels as well
                    if (pixel == 1) energy += frame_hist_.GetBinContent(pixel + 1);
                    else if (pixel == gotthard_constants::kNumberOfChannels) energy += frame_hist_.GetBinContent(pixel - 1);
                    else {
                        energy += frame_hist_.GetBinContent(pixel + 1) + frame_hist_.GetBinContent(pixel - 1);
                    }
                    spectrum_.Fill(energy);
                    single_pixel_spectrum_[pixel - 1].Fill(energy);
                }
            }
        }
    }
    return 0;
}
