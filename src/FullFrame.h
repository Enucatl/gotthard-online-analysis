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
    void merge_packets(const Packet& p1, const Packet& p2);

private:
    int frame1_number_;
    int frame2_number_;
    std::vector<unsigned int> pixels_;
};

#endif /* end of include guard: FULLFRAME_H */
