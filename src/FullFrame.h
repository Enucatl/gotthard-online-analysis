#ifndef FULLFRAME_H
#define FULLFRAME_H

#include <vector>
#include <algorithm>
#include "gotthard_constants.h"
#include "Packet.h"

class FullFrame {

public:
    FullFrame();
    ~FullFrame();
    void read_next_complete_frame(std::istream& f, Packet& p1);
    int frame1_number() const { return frame1_number_; }
    int frame2_number() const { return frame2_number_; }
    unsigned int get_pixel(int i) const { return pixels_.at(i); }
    int size() const { return pixels_.size(); }

private:
    int frame1_number_;
    int frame2_number_;
    std::vector<unsigned int> pixels_;
};

#endif /* end of include guard: FULLFRAME_H */
