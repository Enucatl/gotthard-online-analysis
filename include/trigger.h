#ifndef TRIGGER_H
#define TRIGGER_H

/* Triggers the frames, by calculating the subtraction with respect to a
 * pedestal.
 * The frame values are changed, and only the subtracted values exist after
 * this point.
 * Also returns the original frame, where the bins that have triggered have
 * been modified and reset to the pedestal value. This is used to update the
 * pedestal.
 *
 */

namespace gotthard {
namespace trigger {

const double kThresholdLow = 200;
const double kThresholdMid = 300;
const double kThresholdHigh = 400;

}
}

#endif /* end of include guard: TRIGGER_H */
