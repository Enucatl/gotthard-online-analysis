#include "FrameROOTFunctions.h"

namespace frame_root_functions {

int get_histogram(const FullFrame& frame, TH1D& histogram) {
    if (frame.size() != histogram.GetNbinsX()) return -1;
    //ignore noisy channels
    for (int i = 0; i < gotthard_constants::kMinUsefulChannel; i++) {
        histogram.SetBinContent(i + 1, 0);
    }
    for (int i = gotthard_constants::kMinUsefulChannel; i < gotthard_constants::kMaxUsefulChannel; i++) {
        histogram.SetBinContent(i + 1, frame.get_pixel(i));
    }
    for (int i = gotthard_constants::kMaxUsefulChannel; i < frame.size(); i++) {
        histogram.SetBinContent(i + 1, 0);
    }
    return 0;
}

int get_average(const std::vector<FullFrame>& frames, TH1D& histogram){
    //check that the vector is not empty
    int number_of_frames = frames.size();
    if (!number_of_frames) return -1;

    //check that the histogram has the right number of bins
    int number_of_pixels = frames[0].size();
    if (number_of_pixels != histogram.GetNbinsX()) {
        //std::cerr << number_of_pixels << " " << histogram.GetNbinsX() << std::endl;
        return -2;
    }

    for (int i = 0; i < number_of_pixels; i++) {
        std::vector<unsigned int> values;
        values.reserve(number_of_frames);
        for (std::vector<FullFrame>::const_iterator frame = frames.begin(); frame != frames.end(); ++frame) {
            values.push_back(frame->get_pixel(i));
        }
        double mean = TMath::Mean(values.begin(), values.end());
        //error on the mean: sigma / sqrt(n)
        double error = TMath::RMS(values.begin(), values.end()) / TMath::Sqrt(number_of_frames);
        histogram.SetBinContent(i + 1, mean);
        histogram.SetBinError(i + 1, error);
    }
    return 0;
}

int get_histogram_subtracting_pedestal(const FullFrame& frame, TH1D& pedestal, TH1D& histogram) {
    int result = get_histogram(frame, histogram);
    if (!result)
        histogram.Sumw2();
        histogram.Add(&pedestal, -1);
    return result;
}

int get_histograms_subtracting_pedestal(const std::vector<FullFrame>& frames, TH1D& pedestal, std::vector<TH1D>& histograms) {
    //histograms should be empty
    histograms.clear();
    histograms.reserve(frames.size());
    for (std::vector<FullFrame>::const_iterator frame = frames.begin(); frame != frames.end(); ++frame) {
        std::stringstream histogram_name;
        histogram_name << "frame" << frame->frame1_number() << frame->frame2_number();
        TH1D histogram(
                histogram_name.str().c_str(),
                histogram_name.str().c_str(),
                frame->size(),
                0,
                frame->size());
        int result = get_histogram_subtracting_pedestal(*frame, pedestal, histogram);
        if (result != 0) return result;
        histograms.push_back(histogram);
    }
    return 0;
}

void get_photon_tree(const std::vector<TH1D>& histograms, TTree& tree){
    double energy;
    tree.Branch("energy", &energy, "energy/D");
    for (std::vector<TH1D>::const_iterator histogram = histograms.begin(); histogram != histograms.end(); ++histogram) {
        for (int i = 0; i < histogram->GetNbinsX(); i++) {
            energy = histogram->GetBinContent(i + 1);
            tree.Fill();
        }
    }
}

void get_photon_tree_rebinning(std::vector<TH1D>& histograms, int rebinning, TTree& tree){
    double energy;
    tree.Branch("energy", &energy, "energy/D");
    for (std::vector<TH1D>::iterator histogram = histograms.begin(); histogram != histograms.end(); ++histogram) {
        histogram->Rebin(rebinning);
        std::cout << "";
        for (int i = 0; i < histogram->GetNbinsX(); i++) {
            energy = histogram->GetBinContent(i + 1);
            tree.Fill();
        }
    }
}

int subtract_common_mode(TH1D& histogram, float* peak_positions, int number_of_peaks){
    double integral = histogram.Integral();
    int number_of_empty_bins = histogram.GetNbinsX();
    for (int i = 0; i < number_of_peaks; i++) {
        int bin = 1 + static_cast<int>(peak_positions[i] + 0.5);
        integral -= histogram.GetBinContent(bin);
        number_of_empty_bins--;
        if (bin == 1){
            integral -= histogram.GetBinContent(bin + 1);
            number_of_empty_bins--;
        }
        else if (bin == histogram.GetNbinsX()){
            integral -= histogram.GetBinContent(bin - 1);
            number_of_empty_bins--;
        }
        else {
            integral -= histogram.GetBinContent(bin + 1);
            integral -= histogram.GetBinContent(bin - 1);
            number_of_empty_bins -= 2;
        }
    }
    double common_mode = integral / number_of_empty_bins;
    for (int i = 0; i < histogram.GetNbinsX(); i++) {
        double content = histogram.GetBinContent(i + 1);
        histogram.SetBinContent(i + 1, content - common_mode);
    }
    return 0;
}

}
