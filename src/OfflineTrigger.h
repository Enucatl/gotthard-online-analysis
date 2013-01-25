#ifndef OFFLINETRIGGER_H
#define OFFLINETRIGGER_H

#include <iostream>
#include <string>
#include "Packet.h"
#include "FullFrame.h"

class OfflineTrigger {
public:
    OfflineTrigger(std::string prefix, int run_number);
    ~OfflineTrigger();

private:
    TFile file_;
    TTree tree_;
    FullFrame frame_;
    PedestalCalculator pedestal_calculator_;
};
#endif /* end of include guard: OFFLINETRIGGER_H */
