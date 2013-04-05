#include "pedestal_calculator.h"

namespace gotthard {

bool PedestalCalculator::push(const Frame& frame) {
    frames_.push(frame);
    //add new frame
    //using boost::bind to compose the standard plus and multiplies
    //functions from <functional>:
    //bind(f, bind(g, _1))(x) means
    //f(g(x))
    //in this case, the operation is _1 + factor_ * _2
    //see http://www.boost.org/doc/libs/1_53_0/libs/bind/bind.html#nested_binds
    std::transform(
            current_pedestal_.begin(), current_pedestal_.end(),
            frame.begin(),
            current_pedestal_.begin(),
            boost::bind(
                std::plus<double>(),
                    _1,
                    boost::bind(
                        std::multiplies<double>(), factor_, _2)));
    if (size() > number_of_frames_) {
        //subtract old frame
        //same algorithm as in the comment for the sum
        std::transform(
                current_pedestal_.begin(), current_pedestal_.end(),
                frames_.front().begin(),
                current_pedestal_.begin(),
                boost::bind(std::minus<double>(), _1, boost::bind(std::multiplies<double>(), factor_, _2)));
        frames_.pop();
    }
    return false;
}

}
