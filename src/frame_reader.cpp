#include "frame_reader.h"

namespace gotthard {

std::ifstream& FrameReader::read_next_frame(std::ifstream& file, int& frame_id, std::vector<double>& frame){
    int first_frame_id = -1;
    int second_frame_id = -1;

    //get reference to the two halves (packets)
    std::vector<double>::iterator begin_first_half = frame.begin();
    std::vector<double>::iterator begin_second_half = frame.begin() + gotthard::kHalfNumberOfChannels; 
    std::vector<double>::iterator end_second_half = frame.end(); 

    first_frame_id = read_id(file);
    frame_id = first_frame_id;
    read_packet(file, begin_first_half, begin_second_half);
    if (not file) return file; //return if reading packet was bad (usually EOF reached)

    //the two packets must have consecutive frame numbers
    //and the second one must be event
    //write on the first half and move on with the file
    //until both conditions are satisfied
    second_frame_id = read_id(file);
    while (second_frame_id % 2 or second_frame_id != first_frame_id + 1) {
        first_frame_id = second_frame_id;
        read_packet(file, begin_first_half, begin_second_half);
        if (not file) return file; //return if reading packet was bad (usually EOF reached)
        second_frame_id = read_id(file);
    }

    read_packet(file, begin_second_half, end_second_half);
    if (not file) return file; //return if reading packet was bad (usually EOF reached)
    frame_id = first_frame_id;
    return file;
}

std::ifstream& FrameReader::read_packet(std::ifstream& file, std::vector<double>::iterator& begin, std::vector<double>::iterator& end) {
    for (std::vector<double>::iterator iter = begin; iter != end; ++iter) {
        unsigned int value;
        file.read(reinterpret_cast<char*>(&value), gotthard::kSizeOfStripValue);
        *iter = value;
    }
    file.seekg(gotthard::kSizeOfStripValue, std::ios_base::cur); //skip empty bytes
    return file;
}

int FrameReader::read_id(std::ifstream& file) {
    int frame_id;
    if (file.read(reinterpret_cast<char*>(&frame_id), sizeof(int)))
        return frame_id;
    else
        return -1;
}

}
