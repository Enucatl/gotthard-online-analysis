#ifndef TRIGGER_H
#define TRIGGER_H

/* collection of functions to trigger the frames.
 * they return true if the frame is accepted,
 * false if it's rejected.
 *
 * Thresholds are defined here
 */

namespace gotthard {
namespace trigger {

const double kThresholdLow = 200;
const double kThresholdMid = 300;
const double kThresholdHigh = 400;

bool threshold();

}
}
#endif /* end of include guard: TRIGGER_H */
