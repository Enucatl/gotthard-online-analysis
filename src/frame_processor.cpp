#include "frame_processor.h"

namespace gotthard {

FrameProcessor::process(std::vector<fs::path> input_files) {
        for (std::vector<fs::path>::const_iterator file_name = input_files.begin(); file_name != input_files.end(); ++file_name) {
            ifstream file(file_name.str().c_str());
            read_next_frame(file, frame_id_, frame_);
            subtract(frame_, get_pedestal());
            push(frame_);
            swap_with_subtracted(frame_);
            Fill();
        }
}

}
