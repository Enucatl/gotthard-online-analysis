#!/usr/bin/env python
from __future__ import division, print_function

import sys
import ROOT

channels = 1280
ROOT.gROOT.ProcessLine(".L Packet.cpp+")
ROOT.gROOT.ProcessLine(".L FullFrame.cpp+")
ROOT.gROOT.ProcessLine(".L FrameLoader.cpp+")
ROOT.gROOT.ProcessLine(".L PedestalCalculator.cpp+")
ROOT.gInterpreter.GenerateDictionary("vector<FullFrame>","FullFrame.h;vector");

file_name = sys.argv[1]
run_number = int(sys.argv[2])
print(file_name)
frame_loader = ROOT.FrameLoader(file_name, run_number)
pedestal_frames = 100
pedestal_calculator = ROOT.PedestalCalculator(file_name, run_number,
        0, pedestal_frames)
pedestal_calculator.calculate_pedestal()
pedestal_histogram = pedestal_calculator.get_histogram()
frame = ROOT.FullFrame()
frame_loader.readFrame(pedestal_frames, frame)

number_of_frames = 2000
frames = ROOT.vector("FullFrame")()
frame_loader.readFrames(0, pedestal_frames + number_of_frames, frames)
histogram = ROOT.TH1D("histogram", "histogram", 1280, 0, 1280)
for i in range(channels):
    histogram.SetBinContent(i + 1, frame.get_pixel(i))

histogram.Add(pedestal_histogram, -1)
histogram.Draw()

energy_distribution = ROOT.TTree(
        "energy_distribution",
        "energy distribution")
content = ROOT.std.vector("int")(1, 0)
energy_distribution.Branch("energy", content)

for i in range(frames.size()):
    for j in range(channels):
        content[0] = frames[i].get_pixel(j)
        energy_distribution.Fill()

energy_canvas = ROOT.TCanvas(
    "energy canvas",
    "energy canvas")
energy_distribution.Draw("energy", "energy < 20000")
output_root_file_name = "out.root"
output_root_file = ROOT.TFile(output_root_file_name, "recreate")
output_root_file.cd()
energy_distribution.Write()
raw_input()
