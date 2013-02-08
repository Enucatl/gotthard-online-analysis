#include "trigger.h"

namespace gotthard {
namespace trigger {

bool Trigger::operator()(Frame& frame, const Frame& pedestal) {
    bool triggered = false;
    Frame::iterator subtracted_iter = subtracted_frame_.begin();
    Frame::const_iterator pedestal_iter = pedestal.begin();
    for (Frame::iterator i = frame.begin(); i != frame.end(); ++i, ++pedestal_iter, ++subtracted_iter) {
        *subtracted_iter = *i - *pedestal_iter;
        if (*subtracted_iter > threshold_) {
            triggered = true;
            //value of passed frame changes!
            *i = *pedestal_iter;
        }
    }
    return triggered;
}

}
}
