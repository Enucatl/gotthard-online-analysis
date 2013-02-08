#ifndef RANDOM_SUFFIX_H
#define RANDOM_SUFFIX_H

#include <string>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace gotthard {
namespace random {
std::string get_random_suffix(int length);

}
}

#endif /* end of include guard: RANDOM_SUFFIX_H */
