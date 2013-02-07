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

namespace gotthard {

class PedestalCalculator : public boost::noncopyable {

public:
    PedestalCalculator(int number_of_frames, int number_of_strips):
        number_of_frames_(number_of_frames),
        factor_(1.0 / number_of_frames),
    current_pedestal_(number_of_strips, 0) {}
    int size() { return frames_.size(); }
    bool empty() { return frames_.empty(); }
    bool push(const std::vector<double>& frame);
    const::std::vector<double>& get_pedestal() { return current_pedestal_; }

private:
    int number_of_frames_;
    double factor_;
    std::queue< std::vector<double> > frames_;
    std::vector<double> current_pedestal_;
};
}

#endif /* end of include guard: PEDESTAL_CALCULATOR_H */
