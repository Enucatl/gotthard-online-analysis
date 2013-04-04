#ifndef TRIGGER_H
#define TRIGGER_H

/* ThresholdTriggers the frames, by calculating the subtraction with respect to a
 * pedestal.
 * The frame values are changed, and only the subtracted values exist after
 * this point.
 * Also returns the original frame, where the bins that have triggered have
 * been modified and reset to the pedestal value. This is used to update the
 * pedestal.
 *
 */

#include <vector>

#include "frame.h"

namespace gotthard {
namespace trigger {

const double kThresholdLow = 200;
const double kThresholdMid = 300;
const double kThresholdHigh = 400;

class ThresholdTrigger {
public:
    ThresholdTrigger(int number_of_strips, double threshold) :
        threshold_(threshold),
        subtracted_frame_(number_of_strips, 0) {}
    ~ThresholdTrigger() {};
    //the frame is changed so that the bins that triggered (i.e. whose
    //difference with respect to the pedestal is larger than threshold_) are
    //reset to the average pedestal value.
    //This is done to avoid a bias in the calculation of the pedestal given
    //by strips that detected a real photon and have a large value
    bool operator()(Frame& frame, const Frame& pedestal);

    //swap frame with the latest subtracted frame stored in the trigger
    //this subtracted frame is meant to be then saved in a tree or further
    //processed
    void swap_with_subtracted(Frame& frame) { subtracted_frame_.swap(frame); }

private:
    double threshold_;
    Frame subtracted_frame_;
};
}
}

#endif /* end of include guard: TRIGGER_H */
