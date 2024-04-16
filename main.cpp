#include <fstream>
#include <iostream>
#include "frame.hpp"

int main(int argc, char* argv[])
{
    size_t w = 0;
    size_t h = 0;
    std::ifstream fsIn;
    std::ofstream fsOut;
    frame::Frame* frmIn = nullptr;
    frame::Frame* frmOut = nullptr;
    size_t padding = 0;

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
                frmIn = CREATE_FRAME(NV12, w, h, Input);
            }
            else if (std::strcmp(argv[i], "-i:p010") == 0)
            {
                frmIn = CREATE_FRAME(P010, w, h, Input);
            }
            else if (std::strcmp(argv[i], "-i:y410") == 0)
            {
                frmIn = CREATE_FRAME(Y410, w, h, Input);
            }
            else if (std::strcmp(argv[i], "-i:i420") == 0)
            {
                frmIn = CREATE_FRAME(I420, w, h, Input);
            }
            else if (std::strcmp(argv[i], "-i:y210") == 0)
            {
                frmIn = CREATE_FRAME(Y210, w, h, Input);
            }
            fsIn.open(argv[++i], std::ios::in | std::ios::binary);
        }
        else if (std::strncmp(argv[i], "-o", 2) == 0)
        {
            if (std::strcmp(argv[i], "-o:nv12") == 0)
            {
                frmOut = CREATE_FRAME(NV12, w, h, Output);
            }
            else if (std::strcmp(argv[i], "-o:p010") == 0)
            {
                frmOut = CREATE_FRAME(P010, w, h, Output);
            }
            else if (std::strcmp(argv[i], "-o:y410") == 0)
            {
                frmOut = CREATE_FRAME(Y410, w, h, Output);
            }
            else if (std::strcmp(argv[i], "-o:i420") == 0)
            {
                frmOut = CREATE_FRAME(I420, w, h, Output);
            }
            else if (std::strcmp(argv[i], "-o:y210") == 0)
            {
                frmOut = CREATE_FRAME(Y210, w, h, Output);
            }
            fsOut.open(argv[++i], std::ios::out | std::ios::binary);
        }
        else if (std::strcmp(argv[i], "-p") == 0 ||
            std::strcmp(argv[i], "--padding") == 0)
        {
            padding = strtoull(argv[++i], nullptr, 10);
        }
    }

    frmIn->Allocate();

    if (!frmIn || !frmOut || !fsIn || !fsOut)
    {
        return -1;
    }

    auto bufIn = new char[frmIn->FrameSize(false)];
    auto bufOut = new char[frmOut->FrameSize(true)];

    while (fsIn.read(bufIn, frmIn->FrameSize(false)))
    {
        frmIn->ReadFrame(bufIn);
        frmOut->ConvertFrom(*frmIn);
        frmOut->WriteFrame(bufOut);
        fsOut.write(bufOut, frmOut->FrameSize(true));
    }

    return 0;
}
