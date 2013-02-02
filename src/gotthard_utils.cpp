#include "gotthard_utils.h"

namespace gotthard_utils {

std::string file_name(std::string prefix, int run_number, int file_number, bool is_gui_set) {
    std::stringstream filename;
    filename << prefix;
    if(is_gui_set){
        filename << "f";
        filename.fill('0');
        filename.width(12);
        filename << file_number * gotthard_constants::kFramesPerFile;
        filename << "_" << run_number << ".raw";
        if(boost::filesystem::exists(filename.str())) {
            return filename.str();
        }
        else {
            std::cerr << "file not found! " << filename.str() << std::endl;
        }
    } else {
        filename.fill('0');
        filename.width(9);
        filename << file_number;
        filename << ".dat";
        if(boost::filesystem::exists(filename.str())) {
            return filename.str();
        }
        else {
            std::cerr << "FrameLoader::openFile: *.dat doesnt work" << std::endl;
            std::stringstream new_filename;
            new_filename.fill('0');
            new_filename.width(9);
            new_filename << file_number;
            new_filename << ".raw";
            if(boost::filesystem::exists(new_filename.str())){
                return new_filename.str();
            }
            else {
                return "";
            }
        }
    }
    return "";
}

bool open_file(std::string file_name, std::ifstream& is) {
    is.open(file_name.c_str(), std::ios::binary);
    return is.good();
}

}
