#ifndef FRAMEREADER_H
#define FRAMEREADER_H

/*
 * FrameReader has a read_next_frame method that reads the next full frame from a
 * .raw file saved by the gotthard detector.
 * It doesn't have the resposibility of opening the file or allocating the
 * vector in which to save the data.
 * The vector MUST have the correct size.
 *
 * TODO: 
 *      implement ROI [roi_min, roi_max)
 * */

#include <iostream>
#include <fstream>
#include <vector>

#include "constants.h"
#include "frame.h"

namespace gotthard {

class FrameReader {
public:
    FrameReader(int roi_min, int roi_max):
        roi_min_(roi_min),
        roi_max_(roi_max) {};
    ~FrameReader() {};

    //read next frame inside a file. It is the resposibility of the caller
    //to open a new file if this function returns false or a bad frame_id
    //The frame_id is stored in the frame_id reference
    //The values are stored in the vector frame (that MUST have the correct
    //size beforehand: no error checking is performed!)
    std::ifstream& read_next_frame(std::ifstream& file, int& frame_id, Frame& frame);

private:
    //two utility member functions
    int read_id(std::ifstream& file);

    std::ifstream& read_packet(std::ifstream& file, Frame::iterator& begin, Frame::iterator& end);

    //data members
    int roi_min_;
    int roi_max_;
};

}
#endif /* end of include guard: FRAMEREADER_H */
