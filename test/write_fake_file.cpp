#include <iostream>
#include <fstream>
#include <vector>

#include "constants.h"

const int n = gotthard::kNumberOfChannels;

int main(int argc, const char **argv) {

    std::ofstream output(argv[1], std::ios::binary);
    int pedestal_frame_number = 1;
    std::vector<double> pedestal_frame(n, 100);
    output.write(reinterpret_cast<char*>(&pedestal_frame_number),
            sizeof(int));
    for (std::vector<double>::const_iterator i = pedestal_frame.begin(); i != pedestal_frame.begin() + gotthard::kHalfNumberOfChannels; ++i) {
        unsigned int value = *i;
        output.write(reinterpret_cast<char*>(&value),
                gotthard::kSizeOfStripValue);
    }
    unsigned int value = 0;
    output.write(reinterpret_cast<char*>(&value),
            gotthard::kSizeOfStripValue);
    pedestal_frame_number++;
    output.write(reinterpret_cast<char*>(&pedestal_frame_number),
            sizeof(int));
    for (std::vector<double>::const_iterator i = pedestal_frame.begin() + gotthard::kHalfNumberOfChannels; i != pedestal_frame.end(); ++i) {
        unsigned int value = *i;
        output.write(reinterpret_cast<char*>(&value),
                gotthard::kSizeOfStripValue);
    }
    output.write(reinterpret_cast<char*>(&value),
            gotthard::kSizeOfStripValue);

    int frame_number = 3;
    std::vector<double> frame(n, 150);
    frame.at(2) = 900;
    output.write(reinterpret_cast<char*>(&frame_number),
            sizeof(int));
    for (std::vector<double>::const_iterator i = frame.begin(); i != frame.begin() + gotthard::kHalfNumberOfChannels; ++i) {
        unsigned int value = *i;
        output.write(reinterpret_cast<char*>(&value),
                gotthard::kSizeOfStripValue);
    }
    output.write(reinterpret_cast<char*>(&value),
            gotthard::kSizeOfStripValue);
    frame_number++;
    output.write(reinterpret_cast<char*>(&frame_number),
            sizeof(int));
    for (std::vector<double>::const_iterator i = frame.begin() + gotthard::kHalfNumberOfChannels; i != frame.end(); ++i) {
        unsigned int value = *i;
        output.write(reinterpret_cast<char*>(&value),
                gotthard::kSizeOfStripValue);
    }
    output.write(reinterpret_cast<char*>(&value),
            gotthard::kSizeOfStripValue);

    return 0;
}

