#include "frame_reader.h"

namespace gotthard {

int FrameReader::read_next_frame(std::ifstream& file, int& frame_id, std::vector<double>& frame){
    file.read(reinterpret_cast<char*>(&frame_id), sizeof(int));
    for (int i = 0; i < gotthard::kHalfNumberOfChannels; i++) {
        unsigned int value;
        file.read(reinterpret_cast<char*>(&value), 2);
        frame[i] = value;
    }
    file.seekg(2, std::ios_base::cur);
    int second_frame_id;
    file.read(reinterpret_cast<char*>(&second_frame_id), sizeof(int));
    for (int i = gotthard::kHalfNumberOfChannels; i < gotthard::kNumberOfChannels; i++) {
        unsigned int value;
        file.read(reinterpret_cast<char*>(&value), 2);
        frame[i] = value;
    }
    return 0;
}

}
