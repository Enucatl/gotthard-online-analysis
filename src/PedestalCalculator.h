#include <string>
#include "TH1.h"
#include "TFile.h"
#include "FrameLoader.h"

class PedestalCalculator {
public:
    PedestalCalculator(
            std::string file_prefix,
            int trial_number,
            int start_frame_number,
            int amount_of_full_frames);
    ~PedestalCalculator();
    void calculate_pedestal();
    int save_histogram(std::string output_name);
    const TH1D& get_histogram() { return pedestal_histogram_; }
private:
    std::vector<FullFrame*> full_frames_;
    FrameLoader frame_loader_;
    TH1D pedestal_histogram_;
};
