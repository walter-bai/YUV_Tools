#include <fstream>
#include <iostream>
#include "frame.hpp"

static size_t w = 0;
static size_t h = 0;
static std::ifstream fsIn;
static std::ofstream fsOut;
static frame::Frame* frmIn = nullptr;
static frame::Frame* frmOut = nullptr;
static size_t alignment = 2;
static bool replicate = false;

int ParseArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    if (ParseArgs(argc, argv) != 0)
    {
        return -1;
    }

    frmIn->Allocate();
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

void ParseFrameType(frame::Frame** frm, const char* type, const char* name);

static int ParseArgs(int argc, char* argv[])
{
    for (auto i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "-w") == 0 ||
            std::strcmp(argv[i], "--width") == 0)
        {
            w = strtoull(argv[++i], nullptr, 10);
        }
        else if (std::strcmp(argv[i], "-h") == 0
            || std::strcmp(argv[i], "--height") == 0)
        {
            h = strtoull(argv[++i], nullptr, 10);
        }
        else if (std::strncmp(argv[i], "-i:", 2) == 0)
        {
            ParseFrameType(&frmIn, argv[i] + 3, "Input");
            fsIn.open(argv[++i], std::ios::in | std::ios::binary);
        }
        else if (std::strncmp(argv[i], "-o:", 2) == 0)
        {
            ParseFrameType(&frmOut, argv[i] + 3, "Output");
            fsOut.open(argv[++i], std::ios::out | std::ios::binary);
        }
        else if (std::strcmp(argv[i], "-a") == 0 ||
            std::strcmp(argv[i], "--align") == 0)
        {
            alignment = strtoull(argv[++i], nullptr, 10);
        }
        else if (std::strcmp(argv[i], "-r") == 0 ||
            std::strcmp(argv[i], "--replicate") == 0)
        {
            replicate = !!strtoull(argv[++i], nullptr, 10);
        }
    }

    if (!frmIn || !frmOut || !fsIn || !fsOut)
    {
        return -1;
    }

    frame::Frame::EnableLog(true);
    frmIn->SetPadding(alignment, replicate);
    frmOut->SetPadding(alignment, replicate);

    return 0;
}

static void ParseFrameType(frame::Frame** frm, const char* type, const char* name)
{
    if (std::strcmp(type, "nv12") == 0)
    {
        *frm = CREATE_FRAME(NV12, w, h, name);
    }
    else if (std::strcmp(type, "p010") == 0)
    {
        *frm = CREATE_FRAME(P010, w, h, name);
    }
    else if (std::strcmp(type, "y410") == 0)
    {
        *frm = CREATE_FRAME(Y410, w, h, name);
    }
    else if (std::strcmp(type, "i420") == 0)
    {
        *frm = CREATE_FRAME(I420, w, h, name);
    }
    else if (std::strcmp(type, "y210") == 0)
    {
        *frm = CREATE_FRAME(Y210, w, h, name);
    }
    else if (std::strcmp(type, "grey") == 0)
    {
        *frm = CREATE_FRAME(GREY, w, h, name);
    }
}
