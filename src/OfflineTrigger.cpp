#include "OfflineTrigger.h"

OfflineTrigger::OfflineTrigger(std::string prefix, int run_number,
        int save_every, int pedestal_min_size, bool delete_files):
    prefix_(prefix),
    extension_(".root"),
    run_number_(run_number),
    save_every_(save_every),
    pedestal_min_size_(pedestal_min_size),
    delete_files_(delete_files),
    file_((prefix + boost::lexical_cast<std::string>(run_number) + extension_).c_str(), "recreate"),
    tree_("frame_tree", "frame_tree")
{
    current_pedestal_.resize(gotthard_constants::kNumberOfChannels, 0);
    tree_.Branch("frame_number", &current_frame_number_, "frame_number/I");
    tree_.Branch("frame", &(frame_.pixels_));
    tree_.Branch("pedestal", &current_pedestal_);
}

OfflineTrigger::~OfflineTrigger() {
    file_.Write();
    file_.Close();
}

int OfflineTrigger::start_triggering() {
    int frames_saved = 0;
    int frame_counter = 0;
    int file_id = 0;
    std::ifstream file;
    std::string file_name = gotthard_utils::file_name(prefix_, run_number_, file_id);
    bool open_success = gotthard_utils::open_file(file_name, file);
    while (open_success) {
        Packet empty_packet;
        frame_.read_next_complete_frame(file, empty_packet);
        bool eof_reached = current_frame_number_ == frame_.frame1_number() or frame_.frame1_number() < 0 ;
        if (eof_reached) {
            //delete file
            //std::cerr << "reached EOF" << std::endl;
            file.close();
            if (delete_files_) {
                std::cout << "DELETING " << file_name << std::endl;
                boost::filesystem::remove(file_name);
            }
            else {
                std::cout << "only echoing rm " << file_name << std::endl;
            }
            file_id++;
            file_name = gotthard_utils::file_name(prefix_, run_number_, file_id);
            open_success = gotthard_utils::open_file(file_name, file);
            continue;
        }
        current_frame_number_ = frame_.frame1_number();
        frame_counter++;
        bool photon_candidate_found = pedestal_calculator_.push(frame_);
        if(pedestal_calculator_.size() < pedestal_min_size_)
            continue;
        else 
            pedestal_calculator_.pop();
        if (photon_candidate_found or not(frame_counter % save_every_)) {
            frames_saved++;
            tree_.Fill();
        }
        if (frame_counter > 10000) {
            std::cout << frame_counter << std::endl;
            std::cout << frames_saved << std::endl;
            return frames_saved;
        }
    }
    return frames_saved;
}

