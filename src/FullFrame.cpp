#include "FullFrame.h"

FullFrame::FullFrame():
frame1_number_(-1),
frame2_number_(-1),
{
}

void FullFrame::merge_packets(const Packet& p1, const Packet& p2) {
    pixels_.reserve(gotthard_constants::kNumberOfChannels);
    std::copy(p1.pixels_.begin(), p1.pixels_.end() - 1, pixels_.begin())
    std::copy(p2.pixels_.begin(), p2.pixels_.end() - 1, pixels_.begin() + gotthard_constants::kHalfNumberOfChannels + 1)
}
