#include "FullFrame.h"

FullFrame::FullFrame():
    frame1_number_(-1),
    frame2_number_(-1)
{
}

FullFrame::~FullFrame() {} 

void FullFrame::read_next_complete_frame(std::istream& is, Packet& p1) {
    if (!p1.is_initialized()) p1.read_packet(is);
    Packet p2;
    p2.read_packet(is);
    while(p2.frame_number() != p1.frame_number() + 1 ||
            p2.frame_number() % 2) {
        p1 = p2;
        if(!p2.read_packet(is)) {
            std::cerr << "FrameLoader::readNextCompleteFrame EOF reached" << std::endl;
            return;
        }
    }
    frame1_number_ = p1.frame_number();
    frame2_number_ = p2.frame_number();
    pixels_.resize(gotthard_constants::kNumberOfChannels, 0);
    std::copy(p1.pixels_.begin(), p1.pixels_.end() - 1, pixels_.begin());
    std::copy(p2.pixels_.begin(), p2.pixels_.end() - 1, pixels_.begin() + gotthard_constants::kHalfNumberOfChannels + 1);
}
