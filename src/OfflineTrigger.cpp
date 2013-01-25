#include "OfflineTrigger.h"

OfflineTrigger::OfflineTrigger(std::string prefix, int run_number,
        int save_every, int pedestal_min_size):
    prefix_(prefix),
    extension_(".root"),
    run_number_(run_number),
    save_every_(save_every),
    pedestal_min_size_(pedestal_min_size),
    file_((prefix + extension_).c_str(), "recreate"),
    tree_("frame_tree", "frame_tree")
{
        tree_.Branch("frame_number", &current_frame_number_, "frame_number/D");
        tree_.Branch("frame", &(frame_.pixels_));
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
    std::string file_name = gotthard_utils::file_name(prefix_, file_id);
    bool open_success = gotthard_utils::open_file(file_name, file);
    while (open_success) {
        Packet empty_packet;
        frame_.read_next_complete_frame(file, empty_packet);
        current_frame_number_ = frame_.frame1_number();
        if (frame_.frame1_number() < 0) {
            std::cerr << "reached EOF" << std::endl;
            file.close();
            std::cout << "rm " << file_name << std::endl;
            file_id++;
            file_name = gotthard_utils::file_name(prefix_, file_id);
            open_success = gotthard_utils::open_file(file_name, file);
            continue;
        }
        frame_counter++;
        if(pedestal_calculator_.size() < pedestal_min_size_) {
            continue;
        }
        else {
            pedestal_calculator_.pop();
            pedestal_calculator_.get_pedestal(current_pedestal_);
            bool photon_candidate_found = false;
            for (int i = 0; i < gotthard_constants::kNumberOfChannels; i++) {
                double difference = frame_.get_pixel(i) - current_pedestal_[i];
                if (difference > gotthard_constants::kNoiseThreshold) {
                    photon_candidate_found = true;
                    break;
                }
            }
            if (photon_candidate_found or !(frame_counter % save_every_)) {
                frames_saved++;
                tree_.Fill();
            }
        }
    }
    return frames_saved;
}
