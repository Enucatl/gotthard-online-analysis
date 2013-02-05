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

namespace gotthard {

class FrameReader {
public:
    FrameReader(int roi_min, int roi_max):
        roi_min_(roi_min),
        roi_max_(roi_max) {};

    //returns 0 if fine, 1 if error
    int read_next_frame(std::ifstream& file, int& frame_id, std::vector<double>& frame);

private:
    int roi_min_;
    int roi_max_;
};

}
#endif /* end of include guard: FRAMEREADER_H */
