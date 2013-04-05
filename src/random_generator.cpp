#include "random_generator.h"

namespace gotthard {
namespace random {

const std::string chars(
"abcdefghijklmnopqrstuvwxyz"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"1234567890");
boost::random::random_device rng;
boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);

std::string random_string(int length) {
    std::string random_suffix(length + 1, '0');
    random_suffix[0] = '_';
    for (int i = 0; i < length; i++) {
        random_suffix[i + 1] = chars[index_dist(rng)];
    }
    return random_suffix;
}

}
}
