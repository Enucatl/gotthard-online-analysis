#include "raw_image_tools.h"

namespace gotthard {

bool is_image_file(const boost::filesystem::directory_entry& path){
    //check if a file is a raw image file
    return boost::filesystem::is_regular_file(path) and path.path().extension().string() == ".raw";
}

bool is_file2_newer(const boost::filesystem::path& p1, const boost::filesystem::path& p2){
    return boost::filesystem::last_write_time(p1) < boost::filesystem::last_write_time(p2);
}

void get_all_raw_files(const boost::filesystem::path& folder, std::vector<boost::filesystem::path>& vector){
    //folder MUST exist already, otherwise throws an exception
    boost::filesystem::directory_iterator dir_first(folder), dir_last;
    std::copy(boost::make_filter_iterator(raw_image_tools::is_image_file, dir_first, dir_last),
            boost::make_filter_iterator(raw_image_tools::is_image_file, dir_last, dir_last),
            std::back_inserter(vector)
            );
}

}
