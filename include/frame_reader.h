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
    virtual ~FrameReader() {};

    //Read the next frame from a file. The caller should open the next file
    //if the file stream reaches EOF, or a bad frame id is read.
    //Store the frame_id in the frame_id reference.
    //Store the values in the frame (that MUST have the correct
    //size beforehand: no error checking is performed!).
    //Return the file stream after the reading.
    std::ifstream& read_next_frame(std::ifstream& file, int& frame_id, Frame& frame);

private:
    //Read the frame id.
    int read_id(std::ifstream& file);

    //Read a half frame.
    std::ifstream& read_packet(std::ifstream& file, Frame::iterator& begin, Frame::iterator& end);

    //Minimum and maximum strip for the region of interest.
    int roi_min_;
    int roi_max_;
};

}
#endif /* end of include guard: FRAMEREADER_H */
