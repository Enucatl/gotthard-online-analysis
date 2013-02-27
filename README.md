# Process Gotthard frames

Process the frames recorded by the
[Gotthard](http://www.psi.ch/detectors/gotthard) detector into ROOT TTrees.
The processing includes
    * reading the raw files (FrameReader)
    * subtracting the pedestal (PedestalSubtraction)
    * enforcing a trigger to discard empty frames (Trigger)
    * manage the results and save ROOT files with a TTree containing the
      frames

Efficiency and speed are vital to this code, as the code could ideally be
used to filter and pre-analyse the data online, as they are recorded.
