#include <iostream>
#include <sstream>
#include <string>
#include <boost/program_options.hpp>
#include "OfflineTrigger.h"

namespace po = boost::program_options;

int main(int argc, const char **argv) {

    po::options_description desc("Options");
    desc.add_options()
        ("help", "produce help message")
        ("prefix", po::value<std::string>(), "prefix for saved files")
        ("run_n", po::value<int>(), "run number")
        ("save_every", po::value<int>(), "save every n frames, even if a photon is not detected")
        ("min_pedestal_queue", po::value<int>(), "minimum number of frames to be averaged for the calculation of the pedestal")
        ("really_delete_files", po::value<bool>(), "really delete original files")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    std::string example = "./offline_trigger --prefix=/afs/psi.ch/project/hedpc/raw_data/gotthard/2013.01.24/Nd_ --run_n=0 --save_every=1000 --min_pedestal_queue=1000 --really_delete_files=0";

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        std::cout << example << std::endl;
        return 1;
    }

    std::string prefix;
    int run_n, save_every, min_pedestal_queue;
    bool really_delete_files;
    if (vm.count("prefix") and
            vm.count("run_n") and
            vm.count("save_every") and
            vm.count("min_pedestal_queue") and
            vm.count("really_delete_files")) {
        prefix = vm["prefix"].as<std::string>();
        run_n = vm["run_n"].as<int>();
        save_every = vm["save_every"].as<int>();
        min_pedestal_queue = vm["min_pedestal_queue"].as<int>();
        really_delete_files = vm["really_delete_files"].as<bool>();
        std::cout << prefix << " " << run_n << " " << save_every << " " << min_pedestal_queue << " " << really_delete_files << std::endl;
    }
    else {
        std::cerr << "options missing!" << std::endl;
        std::cerr << desc << std::endl;
        std::cerr << example << std::endl;
        return 2;
    }

    OfflineTrigger trigger(prefix, run_n, save_every, min_pedestal_queue, really_delete_files);
    int saved_frames = trigger.start_triggering();
    std::cout << "saved a total of " << saved_frames << " frames." << std::endl;
    return 0;
}

