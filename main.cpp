#include <fstream>
#include <iostream>
#include "frame.h"

int main(int argc, char* argv[])
{
    size_t w = 0;
    size_t h = 0;
    std::ifstream fsIn;
    std::ofstream fsOut;
    frame::Frame* frmIn = nullptr;
    frame::Frame* frmOut = nullptr;

    for (auto i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "-w") == 0)
        {
            w = strtoull(argv[++i], nullptr, 10);
        }
        else if (std::strcmp(argv[i], "-h") == 0)
        {
            h = strtoull(argv[++i], nullptr, 10);
        }
        else if (std::strncmp(argv[i], "-i", 2) == 0)
        {
            if (std::strcmp(argv[i], "-i:nv12") == 0)
            {
                frmIn = new frame::NV12(w, h);
            }
            else if (std::strcmp(argv[i], "-i:p010") == 0)
            {
                frmIn = new frame::P010(w, h);
            }
            else if (std::strcmp(argv[i], "-i:y410") == 0)
            {
                frmIn = new frame::Y410(w, h);
            }
            else if (std::strcmp(argv[i], "-i:yuv44410p") == 0)
            {
                frmIn = new frame::YUV44410P(w, h);
            }
            else if (std::strcmp(argv[i], "-i:yuv4208p") == 0)
            {
                frmIn = new frame::YUV4208P(w, h);
            }
            frmIn->Allocate();
            fsIn.open(argv[++i], std::ios::in | std::ios::binary);
        }
        else if (std::strncmp(argv[i], "-o", 2) == 0)
        {
            if (std::strcmp(argv[i], "-o:nv12") == 0)
            {
                frmOut = new frame::NV12(w, h);
            }
            else if (std::strcmp(argv[i], "-o:p010") == 0)
            {
                frmOut = new frame::P010(w, h);
            }
            else if (std::strcmp(argv[i], "-o:y410") == 0)
            {
                frmOut = new frame::Y410(w, h);
            }
            else if (std::strcmp(argv[i], "-o:yuv44410p") == 0)
            {
                frmOut = new frame::YUV44410P(w, h);
            }
            else if (std::strcmp(argv[i], "-o:yuv4208p") == 0)
            {
                frmOut = new frame::YUV4208P(w, h);
            }
            fsOut.open(argv[++i], std::ios::out | std::ios::binary);
        }
    }

    if (!frmIn || !frmOut || !fsIn || !fsOut)
    {
        return -1;
    }

    auto bufIn = new char[frmIn->FrameSize()];
    auto bufOut = new char[frmOut->FrameSize()];

    while (fsIn.read(bufIn, frmIn->FrameSize()))
    {
        frmIn->ReadFrame(bufIn);
        frmIn->WriteFrame(bufOut, frmOut->Format());
        fsOut.write(bufOut, frmOut->FrameSize());
    }

    return 0;
}
