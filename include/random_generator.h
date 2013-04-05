#ifndef RANDOM_SUFFIX_H
#define RANDOM_SUFFIX_H

#include <string>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace gotthard {
namespace random {

//Return a random string of given length.
std::string random_string(int length);

}
}

#endif /* end of include guard: RANDOM_SUFFIX_H */
