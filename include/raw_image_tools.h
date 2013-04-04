#ifndef RAW_IMAGE_TOOLS_H_YGVDUMNK
#define RAW_IMAGE_TOOLS_H_YGVDUMNK

#include <boost/iterator/filter_iterator.hpp>
#include <boost/filesystem.hpp>
#include <vector>
#include <iterator>

namespace gotthard {

//check that the file is a valid image file
bool is_image_file(const boost::filesystem::directory_entry& path);

//get all .raw files in a folder
void get_all_raw_files(const boost::filesystem::path& folder, std::vector<boost::filesystem::path>& vector);

}

#endif /* end of include guard: RAW_IMAGE_TOOLS_H_YGVDUMNK */
