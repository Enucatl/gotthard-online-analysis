#include "pedestal_calculator.h"

namespace gotthard {

bool PedestalCalculator::push(const std::vector<double>& frame) {
    frames_.push(frame);
    //add new frame
    //using boost::bind to compose the standard plus and multiplies
    //functions from <functional>:
    //bind(f, bind(g, _1))(x) means
    //f(g(x))
    //see http://www.boost.org/doc/libs/1_53_0/libs/bind/bind.html#nested_binds
    std::transform(
            current_pedestal_.begin(), current_pedestal_.end(),
            frame.begin(),
            current_pedestal_.begin(),
            boost::bind(std::plus<double>(), boost::bind(std::multiplies<double>(), factor_, _2), _1));
    if (size() > number_of_frames_) {
        const std::vector<double>& last = frames_.front();
        frames_.pop();
        //subtract old frame
        //same algorithm as in the comment for the sum
        std::transform(
                current_pedestal_.begin(), current_pedestal_.end(),
                last.begin(),
                current_pedestal_.begin(),
                boost::bind(std::minus<double>(), boost::bind(std::multiplies<double>(), factor_, _2), _1));
    }
    return false;
}

}
