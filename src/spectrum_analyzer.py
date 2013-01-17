#!/usr/bin/env python
from __future__ import division, print_function

import argparse
parser = argparse.ArgumentParser(description='''analyze spectrum recorded by gotthard.''')
parser.add_argument('prefix_data', metavar='DATA_PREFIX',
        nargs=1, help='prefix of the files containing the data')
parser.add_argument('run_number_data', type=int, metavar='RUN_N_DATA',
        nargs=1, help='run number of the data')
parser.add_argument('prefix_pedestal', metavar='PEDESTAL_PREFIX',
        nargs=1, help='prefix of the files containing the pedestal')
parser.add_argument('run_number_pedestal', type=int, metavar='RUN_N_PEDESTAL',
        nargs=1, help='run number of the pedestal')
parser.add_argument('pedestal_frames', type=int, metavar='PEDESTAL_FRAMES',
        nargs=1, help='number of pedestal frames')
parser.add_argument('data_frames', type=int, metavar='DATA_FRAMES',
        nargs=1, help='number of data frames')
parser.parse_args()

import ROOT

ROOT.gROOT.ProcessLine(".L Packet.cpp+")
ROOT.gROOT.ProcessLine(".L FullFrame.cpp+")
ROOT.gROOT.ProcessLine(".L FrameLoader.cpp+")
ROOT.gROOT.ProcessLine(".L FrameROOTFunctions.cpp+")
ROOT.gROOT.ProcessLine(".L gotthard_constants.h+")
ROOT.gInterpreter.GenerateDictionary("vector<FullFrame>","FullFrame.h;vector");
ROOT.gInterpreter.GenerateDictionary("vector<TH1D>","TH1.h;vector");


pedestal = ROOT.TH1D("pedestal", "pedestal",
        ROOT.gotthard_constants.kNumberOfChannels,
        0,
        ROOT.gotthard_constants.kNumberOfChannels
        );
