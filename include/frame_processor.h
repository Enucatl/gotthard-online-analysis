#ifndef FRAME_PROCESSOR_H_26TFI3NG
#define FRAME_PROCESSOR_H_26TFI3NG

#include <vector>
#include <boost/filesystem.hpp>

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
            std::vector<boost::filesystem::path> input_files,
            int roi_min,
            int roi_max,
            int frames_in_pedestal_queue,
            double trigger_threshold,
            int frames_per_file,
            std::string output_prefix
            ):
        Reader(roi_min, roi_max),
        number_of_stripes_(roi_max - roi_min),
        frame_id_(-999),
        frame_(number_of_stripes_, 0),
        PedestalCalculator(frames_in_pedestal_queue,
                number_of_stripes_),
        Trigger(trigger_threshold,
                number_of_stripes_),
        FileManager(frames_per_file,
                output_prefix,
                &frame_id_,
                &frame_) {};
    ~FrameProcessor() {};

private:
    int number_of_stripes_;
    int frame_id_;
    Frame frame_;
};

#endif /* end of include guard: FRAME_PROCESSOR_H_26TFI3NG */

