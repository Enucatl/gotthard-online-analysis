#include "Packet.h"

Packet::Packet():
    frame_number_(-1)
{
}

Packet::Packet(const Packet& other) {
    frame_number_ = other.frame_number_;
    pixels_.reserve(other.pixels_.size());
    std::copy(other.pixels_.begin(), other.pixels_.end(), pixels_.begin());
}

Packet& Packet::operator=(const Packet& other){
    frame_number_ = other.frame_number_;
    pixels_.clear();
    pixels_.reserve(other.pixels_.size());
    std::copy(other.pixels_.begin(), other.pixels_.end(), pixels_.begin());
    return *this;
}

bool Packet::operator==(const Packet& other) const {
    return frame_number_ == other.frame_number_;
}

std::istream& Packet::read_packet(std::istream& is) {
    pixels_.reserve(gotthard_constants::kHalfNumberOfChannels + 1);
    is.read(reinterpret_cast<char*>(&frame_number_),
            sizeof(int));
    int size = gotthard_constants::kBufferLength - sizeof(int);
    return is.read(reinterpret_cast<char*>(&pixels_[0]), size);
}
