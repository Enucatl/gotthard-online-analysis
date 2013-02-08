#include "random_suffix.h"

namespace gotthard {
namespace random {

const std::string chars(
"abcdefghijklmnopqrstuvwxyz"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"1234567890");
boost::random::random_device rng;
boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);

std::string get_random_suffix(int length) {
    std::string random_suffix(length, '0');
    for (int i = 0; i < length; i++) {
        random_suffix[i] = chars[index_dist(rng)];
    }
    return random_suffix;
}

}
}
