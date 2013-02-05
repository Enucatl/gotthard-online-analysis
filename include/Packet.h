#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <fstream>
#include <vector>


#include "gotthard_constants.h"

class FullFrame;

class Packet {
public:
    friend class FullFrame;
    Packet();
    explicit Packet(const Packet& other);
    ~Packet();
    bool operator==(const Packet& other) const;
    Packet& operator=(const Packet& other);
    std::istream& read_packet(std::istream& is);
    bool is_initialized() const { return frame_number_ != -1; }
    int frame_number() const { return frame_number_; }
    unsigned int get_pixel(int i) const { return pixels_.at(i); }
    int size() const { return pixels_.size(); }

private:
    std::vector<unsigned short> pixels_;
    int frame_number_;
};

#endif /* end of include guard: PACKET_H */
