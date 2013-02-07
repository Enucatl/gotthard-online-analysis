#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>

#include <boost/bind.hpp>

int main(int argc, const char **argv) {
    int n = 10;
    std::vector<double> vector(n, 100);
    std::vector<double> to_be_added(n, 1000);

    std::copy(vector.begin(), vector.end(),
            std::ostream_iterator<double>(std::cout, " "));
    std::cout << std::endl;

    std::transform(
            vector.begin(),
            vector.end(),
            to_be_added.begin(),
            vector.begin(),
            boost::bind(std::plus<double>(), boost::bind(std::multiplies<double>(), 0.1, _2), _1));

    std::copy(vector.begin(), vector.end(),
            std::ostream_iterator<double>(std::cout, " "));
    std::cout << std::endl;
    return 0;
}
