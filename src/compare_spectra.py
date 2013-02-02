#!/usr/bin/env python
from __future__ import division, print_function

import os, sys
import argparse
parser = argparse.ArgumentParser(description='''analyze spectrum recorded by gotthard.
        example:
        python spectrum_analyzer.py /afs/psi.ch/project/hedpc/raw_data/gotthard/2013.01.15/Ir_ 2 1000 /afs/psi.ch/project/hedpc/raw_data/gotthard/2013.01.15/flatIr_ 3 1000
''')
parser.add_argument('file1', metavar='FILE2',
        nargs=1, help='file with first spectrum')
parser.add_argument('file2', metavar='FILE2',
        nargs=1, help='file with second spectrum')
args = parser.parse_args()

file_name1 = args.file1[0]
file_name2 = args.file2[0]

import ROOT
from rootstyle import tdrstyle

tdrstyle()

if not os.path.exists(file_name1) or not os.path.exists(file_name2):
    print("file not found!")
    sys.exit(1)

hist_name = "spectrum"
file1 = ROOT.TFile(file_name1)
file2 = ROOT.TFile(file_name2)

spectrum1 = file1.Get(hist_name)
spectrum2 = file2.Get(hist_name)

print("compatibility (Kolmogorov-Smirnov)", spectrum1.KolmogorovTest(spectrum2))
canvas1 = ROOT.TCanvas("canvas1", "canvas1")
canvas1.cd()
spectrum1.Draw()
canvas1.SaveAs(file_name1.replace(".root", ".png"))
canvas2 = ROOT.TCanvas("canvas2", "canvas2")
canvas2.cd()
spectrum2.Draw()
canvas2.SaveAs(file_name2.replace(".root", ".png"))
spectrum1.Add(spectrum2, -1)

canvas = ROOT.TCanvas("difference_canvas", "difference_canvas")
canvas.cd()
spectrum1.Draw()
canvas.SaveAs(file_name1.replace(".root", "") + file_name2.replace(".root",
".png"))
raw_input()
