#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/filesystem.hpp>

#include "gotthard_constants.h"

namespace gotthard_utils {

std::string file_name(std::string prefix, int run_number, int file_number, bool is_gui_set=true);

bool open_file(std::string file_name, std::ifstream& is);

}
