#!/usr/bin/env python
from __future__ import division, print_function

import argparse
parser = argparse.ArgumentParser(description='''display single frame recorded by gotthard.
        example:
        python single_frame_reader.py /afs/psi.ch/project/hedpc/raw_data/gotthard/2013.01.24/Ir_ 2 150000
''')
parser.add_argument('prefix', metavar='PREFIX',
        nargs=1, help='prefix of the files containing the data')
parser.add_argument('run_number', type=int, metavar='RUN_N',
        nargs=1, help='run number')
parser.add_argument('frame_number', type=int, metavar='FRAME_NUMBER',
        nargs=1, help='frame number')
args = parser.parse_args()

prefix = args.prefix[0]
run_number = args.run_number[0]
frame_number = args.frame_number[0]

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
frame_pedestal = ROOT.FullFrame()
frame_loader_data = ROOT.FrameLoader(prefix, run_number)
frame_histogram = ROOT.TH1D("frame", "frame",
        ROOT.gotthard_constants.kNumberOfChannels,
        0,
        ROOT.gotthard_constants.kNumberOfChannels)
pedestal_histogram = ROOT.TH1D("pedestal", "pedestal",
        ROOT.gotthard_constants.kNumberOfChannels,
        0,
        ROOT.gotthard_constants.kNumberOfChannels)

pedestal_calculator = ROOT.PedestalCalculator()
key = "0"
frames_pedestal = 1000

peak_finder = ROOT.TSpectrum(3)
for i in range(frames_pedestal):
    frame_loader_data.readFrame(i, frame_pedestal)
    pedestal_calculator.push(frame_pedestal)
    if pedestal_calculator.size() > frames_pedestal:
        pedestal_calculator.pop()

frame_loader_data.readFrame(frame_number, frame)
ROOT.frame_root_functions.get_histogram(frame, frame_histogram)
pedestal_calculator.get_pedestal(pedestal_histogram)
frame_histogram.Add(pedestal_histogram, -1)
canvas = ROOT.TCanvas("canvas", "canvas")
peak_finder.Search(frame_histogram, 3, "nodraw", 0.4)
frame_histogram.Draw()
key = raw_input("type q to exit: ")
