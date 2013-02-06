#include "pedestal_calculator.h"

namespace gotthard {

namespace average_update {
double plus(double factor, double d1, double d2) {
    return d1 + factor * d2;
}
double minus(double factor, double d1, double d2) {
    return d1 - factor * d2;
}
}

bool PedestalCalculator::push(const std::vector<double>& frame) {
    frames_.push(frame);
    //add new frame
    std::transform(
            current_pedestal_.begin(), current_pedestal_.end(),
            frame.begin(),
            current_pedestal_.begin(),
            boost::bind(&average_update::plus, factor_, _2, _3));
    if (size() > number_of_frames_) {
        const std::vector<double>& last = frames_.front();
        frames_.pop();
        //subtract old frame
        std::transform(
                current_pedestal_.begin(), current_pedestal_.end(),
                last.begin(),
                current_pedestal_.begin(),
                boost::bind(&average_update::minus, factor_, _2, _3));
    }
    return false;
}

}
