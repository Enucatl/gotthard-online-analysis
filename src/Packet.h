#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <vector>

#include "gotthard_constants.h"

class FullFrame;

class Packet {
public:
    friend class FullFrame;
    Packet();
    ~Packet();
    bool operator==(const Packet& other) const;
    std::istream& read_packet(std::istream& is);
    bool is_initialized() const { return frame_number_ != -1; }
    int frame_number() const { return frame_number_; }

private:
    std::vector<unsigned short> pixels_;
    int frame_number_;
};

#endif /* end of include guard: PACKET_H */
