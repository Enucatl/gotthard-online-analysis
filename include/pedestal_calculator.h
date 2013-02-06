#ifndef PEDESTAL_CALCULATOR_H
#define PEDESTAL_CALCULATOR_H

/* calculate pedestal based on the last number_of_frames frames */

#include <iostream>
#include <algorithm>
#include <queue>

#include <boost/bind.hpp>

namespace gotthard {

class PedestalCalculator {

public:
    PedestalCalculator(int number_of_frames, int number_of_strips):
        number_of_frames_(number_of_frames),
        factor_(1.0 / number_of_frames),
    current_pedestal_(number_of_strips, 0) {};
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

namespace average_update {

//functions used to update the mean pedestal
//with std::transform by adding the newest frame
//and subtracting the oldest one

inline double plus(double factor, double d1, double d2);
inline double minus(double factor, double d1, double d2);
}
}

#endif /* end of include guard: PEDESTAL_CALCULATOR_H */
