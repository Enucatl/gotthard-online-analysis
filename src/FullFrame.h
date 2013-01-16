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

private:
    int frame1_number_;
    int frame2_number_;
    std::vector<unsigned int> pixels_;
};

#endif /* end of include guard: FULLFRAME_H */
