#ifndef PEDESTAL_CALCULATOR_H
#define PEDESTAL_CALCULATOR_H

/* calculate pedestal based on the last number_of_frames frames */

#include <iostream>
#include <functional>
#include <algorithm>
#include <queue>

#include <boost/bind.hpp>
//make it noncopyable: it makes no sense, and the queue + vector are very
//expensive to copy
#include <boost/utility.hpp>

#include "frame.h"

namespace gotthard {

class PedestalCalculator : public boost::noncopyable {

public:
    PedestalCalculator(int number_of_frames, int number_of_strips):
        number_of_frames_(number_of_frames),
        factor_(1.0 / number_of_frames),
        current_pedestal_(number_of_strips, 0) {};
    ~PedestalCalculator() {};
    int size() { return frames_.size(); }
    bool empty() { return frames_.empty(); }
    bool push(const Frame& frame);
    const::Frame& get_pedestal() { return current_pedestal_; }

private:
    int number_of_frames_;
    double factor_;
    std::queue<Frame> frames_;
    Frame current_pedestal_;
};
}

#endif /* end of include guard: PEDESTAL_CALCULATOR_H */
