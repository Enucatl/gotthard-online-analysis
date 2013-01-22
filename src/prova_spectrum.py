#!/usr/bin/env python
from __future__ import division, print_function

import argparse
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

pedestal_queue = 1000
pars = 1000, -1000, 5000
bin_width = (pars[2] - pars[1]) / pars[0]
spectrum_analyzer = ROOT.Spectrum(prefix_data,
        run_number_data,
        2,
        *pars)
spectrum_analyzer.set_threshold(300)
spectrum_analyzer.set_queue_size(pedestal_queue)
spectrum_analyzer.read_frames(number_of_frames)
spectrum_histogram = spectrum_analyzer.get_spectrum()
spectrum_histogram.SetTitle(";ADC value;entries / {0:.0f}".format(bin_width))
spectrum_histogram.Draw()

output_file = ROOT.TFile("out.root", "recreate")
output_file.cd()
spectrum_histogram.Write()
raw_input()
