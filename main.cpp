#include <fstream>
#include <iostream>
#include "frame.h"

int main(int argc, char* argv[])
{
    size_t w = 0;
    size_t h = 0;
    FOURCC fmtIn = FOURCC::UNDEF;
    FOURCC fmtOut = FOURCC::UNDEF;
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
                fmtIn = FOURCC::NV12;
            }
            else if (std::strcmp(argv[i], "-i:p010") == 0)
            {
                fmtIn = FOURCC::P010;
            }
            else if (std::strcmp(argv[i], "-i:y410") == 0)
            {
                fmtIn = FOURCC::Y410;
            }
            else if (std::strcmp(argv[i], "-i:yuv44410p") == 0)
            {
                fmtIn = FOURCC::YUV44410P;
            }
            else if (std::strcmp(argv[i], "-i:yuv4208p") == 0)
            {
                fmtIn = FOURCC::YUV4208P;
            }
            else if (std::strcmp(argv[i], "-i:y210") == 0)
            {
                fmtIn = FOURCC::Y210;
            }
            fsIn.open(argv[++i], std::ios::in | std::ios::binary);
        }
        else if (std::strncmp(argv[i], "-o", 2) == 0)
        {
            if (std::strcmp(argv[i], "-o:nv12") == 0)
            {
                fmtOut = FOURCC::NV12;
            }
            else if (std::strcmp(argv[i], "-o:p010") == 0)
            {
                fmtOut = FOURCC::P010;
            }
            else if (std::strcmp(argv[i], "-o:y410") == 0)
            {
                fmtOut = FOURCC::Y410;
            }
            else if (std::strcmp(argv[i], "-o:yuv44410p") == 0)
            {
                fmtOut = FOURCC::YUV44410P;
            }
            else if (std::strcmp(argv[i], "-o:yuv4208p") == 0)
            {
                fmtOut = FOURCC::YUV4208P;
            }
            else if (std::strcmp(argv[i], "-o:y210") == 0)
            {
                fmtOut = FOURCC::Y210;
            }
            fsOut.open(argv[++i], std::ios::out | std::ios::binary);
        }
        else if (std::strcmp(argv[i], "-p") == 0 ||
            std::strcmp(argv[i], "--padding") == 0)
        {
            padding = strtoull(argv[++i], nullptr, 10);
        }
    }

    frmIn = frame::CreateFrame(fmtIn, w, h, padding);
    frmIn->Allocate();
    frmOut = frame::CreateFrame(fmtOut, w, h, padding);

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
