#include "trigger.h"

namespace gotthard {
namespace trigger {

bool operator()(std::vector<double>& frame, const std::vector<double>& pedestal) {
    bool triggered = false;
    std::vector<double>::iterator subtracted_iter = subtracted_frame_.begin();
    std::vector<double>::const_iterator pedestal_iter = pedestal.begin();
    for (std::vector<double>::iterator i = frame.begin(); i != frame.end(); ++i, ++pedestal_iter, ++subtracted_iter) {
        *subtracted_iter = *i - *pedestal_iter;
        if (*subtracted_iter > threshold_) {
            triggered = true;
            //value of passed frame changes!
            *i = *pedestal_iter;
        }
    }
    
}

}
}
