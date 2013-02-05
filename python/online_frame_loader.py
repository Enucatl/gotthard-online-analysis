#!/usr/bin/env python
from __future__ import division, print_function

import argparse
parser = argparse.ArgumentParser(description='''analyze spectrum recorded by gotthard.
        example:
        python spectrum_analyzer.py /afs/psi.ch/project/hedpc/raw_data/gotthard/2013.01.15/Ir_ 2 1000 /afs/psi.ch/project/hedpc/raw_data/gotthard/2013.01.15/flatIr_ 3 1000

''')
parser.add_argument('prefix_data', metavar='DATA_PREFIX',
        nargs=1, help='prefix of the files containing the data')
parser.add_argument('run_number_data', type=int, metavar='RUN_N_DATA',
        nargs=1, help='run number of the data')
args = parser.parse_args()

prefix_data = args.prefix_data[0]
run_number_data = args.run_number_data[0]

import ROOT
from rootstyle import tdrstyle

tdrstyle()
ROOT.gROOT.ProcessLine(".L Packet.cpp+")
ROOT.gROOT.ProcessLine(".L FullFrame.cpp+")
ROOT.gROOT.ProcessLine(".L FrameLoader.cpp+")
ROOT.gROOT.ProcessLine(".L FrameROOTFunctions.cpp+")
ROOT.gROOT.ProcessLine(".L gotthard_constants.h+")
ROOT.gROOT.ProcessLine(".L PedestalCalculator.cpp+")

frame = ROOT.FullFrame()
frame_histogram = ROOT.TH1D("frame", "frame",
        ROOT.gotthard_constants.kNumberOfChannels,
        0,
        ROOT.gotthard_constants.kNumberOfChannels)
pedestal_histogram = ROOT.TH1D("pedestal", "pedestal",
        ROOT.gotthard_constants.kNumberOfChannels,
        0,
        ROOT.gotthard_constants.kNumberOfChannels)

frame_loader_data = ROOT.FrameLoader(prefix_data,
        run_number_data)

pedestal_calculator = ROOT.PedestalCalculator()
key = "0"
frame_number = 0
frames_pedestal = 100

peak_finder = ROOT.TSpectrum(3)
while key != 'q':
    frame_loader_data.readFrame(frame_number, frame)
    pedestal_calculator.push(frame)
    if pedestal_calculator.size() > frames_pedestal:
        pedestal_calculator.pop()
    ROOT.frame_root_functions.get_histogram(frame, frame_histogram)
    pedestal_calculator.get_pedestal(pedestal_histogram)
    frame_number += 1
    canvas = ROOT.TCanvas("canvas", "canvas")
    frame_histogram.Add(pedestal_histogram, -1)
    peak_finder.Search(frame_histogram, 3, "nodraw", 0.4)
    frame_histogram.Draw()
    key = raw_input("type q to exit: ")
