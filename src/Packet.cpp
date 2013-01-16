#include "Packet.h"

Packet::Packet():
    frame_number_(-1)
{
}

bool Packet::operator==(const Packet& other) const {
    return this->frame_number_ == other.frame_number_;
}

std::istream& Packet::read_packet(std::istream& is) {
    pixels_.reserve(gotthard_constants::kHalfNumberOfChannels + 1);
    is.read(reinterpret_cast<char*>(&frame_number_),
            sizeof(int));
    int size = gotthard_constants::kBufferLength - sizeof(int);
    return is.read(reinterpret_cast<char*>(&pixels_[0]), size);
}
