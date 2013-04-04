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
            std::cout << "file name " << file_name->string() << std::endl;
            std::cout << "size of frame " << this->frame_.size() << std::endl;
            this->read_next_frame(file, frame_id_, frame_);
            std::cout << "read one frame " << this->frame_.size() << std::endl;
            this->subtract(frame_, this->get_pedestal());
            std::cout << "subtracted pedestal " << this->frame_.size() << std::endl;
            this->push(frame_);
            std::cout << "pushed onto queue " << this->frame_.size() << std::endl;
            this->swap_with_subtracted(frame_);
            std::cout << "swapped " << this->frame_.size() << std::endl;
            this->Fill();
            std::cout << "filled " << this->frame_.size() << std::endl;
        }
        return 0;
}

}
#endif /* end of include guard: FRAME_PROCESSOR_H_26TFI3NG */

