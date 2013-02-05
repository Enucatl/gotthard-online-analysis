#!/usr/bin/env python
from __future__ import division, print_function

import argparse
import os
parser = argparse.ArgumentParser(description='''analyze spectrum recorded by gotthard.
        example:
        python offline_trigger.py /afs/psi.ch/project/hedpc/raw_data/gotthard/2013.01.18/Ir_ 0 1000 1000
''')
parser.add_argument('prefix', metavar='PREFIX',
        nargs=1, help='prefix of the files containing the data')
parser.add_argument('run_number', type=int, metavar='RUN_N',
        nargs=1, help='run number')
parser.add_argument('save_every', type=int, metavar='SAVE_EVERY',
        nargs=1, help='save every SAVE_EVERY frames, even if a photon is not detected')
parser.add_argument('min_pedestal_size', type=int, metavar='MIN_PEDESTAL',
        nargs=1, help='minimum size of the pedestal queue')
args = parser.parse_args()

prefix = args.prefix[0]
run_number = args.run_number[0]
save_every = args.save_every[0]
min_pedestal_size = args.min_pedestal_size[0]

import ROOT
from rootstyle import tdrstyle

tdrstyle()
ROOT.gROOT.ProcessLine(".L gotthard_utils.cpp+")
ROOT.gROOT.ProcessLine(".L Packet.cpp+")
ROOT.gROOT.ProcessLine(".L FullFrame.cpp+")
ROOT.gROOT.ProcessLine(".L FrameROOTFunctions.cpp+")
ROOT.gROOT.ProcessLine(".L PedestalCalculator.cpp+")
ROOT.gROOT.ProcessLine(".L OfflineTrigger.cpp+")

trigger = OfflineTrigger(prefix, run_number, save_every, min_pedestal_size)
trigger.start_triggering()
