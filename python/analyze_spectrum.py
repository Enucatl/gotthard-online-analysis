#!/usr/bin/env python
from __future__ import division, print_function

import argparse
import os
parser = argparse.ArgumentParser(description='''analyze spectrum recorded by gotthard.
        example:
        python prova_spectrum.py /afs/psi.ch/project/hedpc/raw_data/gotthard/2013.01.18/Ir_ 0 1000 
''')
parser.add_argument('prefix', metavar='DATA_PREFIX',
        nargs=1, help='prefix of the files containing the data')
parser.add_argument('run_number', type=int, metavar='RUN_N_DATA',
        nargs=1, help='run number')
parser.add_argument('n_frames', type=int, metavar='N_FRAMES',
        nargs=1, help='number of frames to be analyzed')
args = parser.parse_args()

prefix_data = args.prefix[0]
run_number_data = args.run_number[0]
number_of_frames = args.n_frames[0]

import ROOT
from rootstyle import tdrstyle

tdrstyle()
ROOT.gROOT.ProcessLine(".L Packet.cpp+")
ROOT.gROOT.ProcessLine(".L FullFrame.cpp+")
ROOT.gROOT.ProcessLine(".L FrameLoader.cpp+")
ROOT.gROOT.ProcessLine(".L FrameROOTFunctions.cpp+")
ROOT.gROOT.ProcessLine(".L PedestalCalculator.cpp+")
ROOT.gROOT.ProcessLine(".L Spectrum.cpp+")
ROOT.gROOT.ProcessLine(".L gotthard_constants.h+")
ROOT.gInterpreter.GenerateDictionary("vector<TH1D>", "TH1.h;vector")


pedestal_queue = 1000
pars = 200, -200, 3800
bin_width = (pars[2] - pars[1]) / pars[0]
spectrum_canvas = ROOT.TCanvas(
        "spectrum_canvas",
        "spectrum_canvas")
max_photons_per_frame = 5
spectrum_analyzer = ROOT.Spectrum(prefix_data,
        run_number_data,
        max_photons_per_frame,
        *pars)
spectrum_analyzer.set_threshold(300)
spectrum_analyzer.set_queue_size(pedestal_queue)
spectrum_analyzer.read_frames(number_of_frames)
spectrum_histogram = spectrum_analyzer.get_spectrum()
spectrum_histogram.SetTitle(";ADC value;entries / {0:.0f}".format(bin_width))
spectrum_histogram.Draw()
spectrum_canvas.SaveAs("~/spectrum.png")

near_pixel_canvas = ROOT.TCanvas(
        "near_pixel_canvas",
        "near_pixel_canvas")
near_pixel = spectrum_analyzer.get_near_pixel_correlation()
near_pixel.SetTitle(";peak ADC value;highest neighbour value")
near_pixel.SetMarkerStyle(1)
near_pixel.SetMarkerSize(0.2)
near_pixel.SetStats()
near_pixel.Draw()
near_pixel_canvas.SaveAs("~/neighbour_strips.png")

output_name = os.path.normpath(prefix_data)
output_name = os.path.basename(output_name)
output_name = "{0}{1}_{2}.root".format(
        output_name,
        run_number_data,
        number_of_frames)
output_file = ROOT.TFile(output_name, "recreate")
output_file.cd()
spectrum_histogram.Write()
near_pixel.Write()
print(spectrum_analyzer.get_empty_frames())
strip_histogram = ROOT.TH1D(
        "photons_per_strip",
        "photons_per_strip;strip number;entries / 1",
        ROOT.gotthard_constants.kNumberOfChannels,
        0,
        ROOT.gotthard_constants.kNumberOfChannels)

for i in range(ROOT.gotthard_constants.kNumberOfChannels):
    single_strip_hist = spectrum_analyzer.get_single_pixel_spectrum(i)
    strip_histogram.SetBinContent(i + 1,
            single_strip_hist.Integral())
    single_strip_hist.Write()

strip_histogram.Write()

strip_canvas = ROOT.TCanvas("strip_canvas", "strip_canvas")
strip_histogram.Draw()
raw_input()
