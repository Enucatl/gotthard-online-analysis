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
parser.add_argument('data_frames', type=int, metavar='DATA_FRAMES',
        nargs=1, help='number of data frames')
parser.add_argument('prefix_pedestal', metavar='PEDESTAL_PREFIX',
        nargs=1, help='prefix of the files containing the pedestal')
parser.add_argument('run_number_pedestal', type=int, metavar='RUN_N_PEDESTAL',
        nargs=1, help='run number of the pedestal')
parser.add_argument('pedestal_frames', type=int, metavar='PEDESTAL_FRAMES',
        nargs=1, help='number of pedestal frames')
args = parser.parse_args()

prefix_pedestal = args.prefix_pedestal[0]
run_number_pedestal = args.run_number_pedestal[0]
n_pedestal_frames = args.pedestal_frames[0]
prefix_data = args.prefix_data[0]
run_number_data = args.run_number_data[0]
n_data_frames = args.data_frames[0]

import ROOT
from rootstyle import tdrstyle

tdrstyle()
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
        )

frame_loader_pedestal = ROOT.FrameLoader(prefix_pedestal,
        run_number_pedestal)
frame_loader_data = ROOT.FrameLoader(prefix_data,
        run_number_data)

frames_pedestal = ROOT.std.vector("FullFrame")()
frame_loader_pedestal.readFrames(0, n_pedestal_frames, frames_pedestal)
result = ROOT.frame_root_functions.get_average(frames_pedestal, pedestal)
pedestal.Draw("e")

frames_data = ROOT.std.vector("FullFrame")()
histograms_data = ROOT.std.vector("TH1D")()
frame_loader_data.readFrames(0, n_data_frames, frames_data)
result = ROOT.frame_root_functions.get_histograms_subtracting_pedestal(
        frames_data,
        pedestal,
        histograms_data)
data_tree = ROOT.TTree("data_tree", "data_tree")
ROOT.frame_root_functions.get_photon_tree_rebinning(histograms_data, 3,
        data_tree)
energy_canvas = ROOT.TCanvas(
        "energy_canvas",
        "energy_canvas")
energy_histogram = ROOT.TH1D(
        "energy_histogram",
        "energy_histogram",
        4000,
        -1000,
        3000)
data_tree.Project("energy_histogram", "energy")
energy_canvas.SetLogy()
energy_histogram.Draw()
raw_input()
