#ifndef FRAME_PROCESSOR_H_26TFI3NG
#define FRAME_PROCESSOR_H_26TFI3NG


template<
    class Frame,
    class Reader,
    class PedestalSubtraction,
    class Trigger,
    class FileManager
    >
class FrameProcessor:
    public Reader,
    public PedestalSubtraction,
    public Trigger,
    public FileManager
{
public:
    FrameProcessor (arguments);
    ~FrameProcessor ();

private:
    /* data */
};

#endif /* end of include guard: FRAME_PROCESSOR_H_26TFI3NG */

