#ifndef FRAME_PROCESSOR_H_26TFI3NG
#define FRAME_PROCESSOR_H_26TFI3NG

#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace gotthard {

template<
    class Frame,
    class Reader,
    class PedestalCalculator,
    class Trigger,
    class FileManager
    >
class FrameProcessor:
    public Reader,
    public PedestalCalculator,
    public Trigger,
    public FileManager
{
public:
    FrameProcessor(
            int roi_min,
            int roi_max,
            int frames_in_pedestal_queue,
            double trigger_threshold,
            int frames_per_file,
            std::string output_prefix
            ):
        Reader(roi_min, roi_max),
        PedestalCalculator(frames_in_pedestal_queue,
                roi_max - roi_min),
        Trigger(roi_max - roi_min,
                trigger_threshold),
        FileManager(frames_per_file,
                output_prefix,
                &frame_id_,
                &frame_),
        number_of_strips_(roi_max - roi_min),
        frame_id_(-999),
        frame_(number_of_strips_, 0) {};
    ~FrameProcessor() {};

    //Process all the input files.
    int operator()(std::vector<fs::path> input_files);
            

private:
    int number_of_strips_;
    int frame_id_;
    Frame frame_;
};

template<
    class Frame,
    class Reader,
    class PedestalCalculator,
    class Trigger,
    class FileManager
    >
int FrameProcessor<Frame, Reader, PedestalCalculator, Trigger, FileManager>::operator()(std::vector<fs::path> input_files) {
        for (std::vector<fs::path>::const_iterator file_name = input_files.begin(); file_name != input_files.end(); ++file_name) {
            std::ifstream file(file_name->c_str());
            if (not this->read_next_frame(file, frame_id_, frame_)) {
                //open the next file if the frame could not be read
                //(usually EOF reached)
                continue;
            }
            bool triggered = this->subtract(frame_, this->get_pedestal());
            bool pedestal_calculated = this->push(frame_);
            if (pedestal_calculated) {
                this->swap_with_subtracted(frame_);
                this->Fill();
            }
            else {
                continue
            }
        }
        return 0;
}

}

#endif /* end of include guard: FRAME_PROCESSOR_H_26TFI3NG */
