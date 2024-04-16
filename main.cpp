#include <algorithm>
#include <fstream>
#include <future>
#include <iostream>
#include <thread>
#include "frame.hpp"

static const size_t coreNum = std::thread::hardware_concurrency();
static size_t w = 0;
static size_t h = 0;
static std::ifstream fsIn;
static std::ofstream fsOut;
static frame::Frame** frmIn = nullptr;
static frame::Frame** frmOut = nullptr;
static size_t alignment = 2;
static bool replicate = false;

int ParseArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    frmIn = new frame::Frame * [coreNum] {nullptr};
    frmOut = new frame::Frame * [coreNum] {nullptr};

    if (ParseArgs(argc, argv) != 0)
    {
        return -1;
    }

    for (size_t i = 0; i < coreNum; i++)
    {
        frmIn[i]->SetPadding(alignment, replicate);
        frmOut[i]->SetPadding(alignment, replicate);
        frmIn[i]->Allocate();
    }

    const size_t frmSzIn = frmIn[0]->FrameSize(false);
    const size_t frmSzOut = frmOut[0]->FrameSize(true);
    auto bufIn = new char[frmSzIn * coreNum];
    auto bufOut = new char[frmSzOut * coreNum];

    while (fsIn.read(bufIn, frmSzIn * coreNum), fsIn.gcount())
    {
        size_t frameNum = std::min(fsIn.gcount() / frmSzIn, coreNum);
        std::vector<std::future<void>> tasks;
        for (size_t i = 0; i < frameNum; i++)
        {
            tasks.push_back(std::async(
                std::launch::async,
                [=]() {
                    frmIn[i]->ReadFrame(bufIn + frmSzIn * i);
                    frmOut[i]->ConvertFrom(*frmIn[i]);
                    frmOut[i]->WriteFrame(bufOut + frmSzOut * i);
                }));
        }
        for (size_t i = 0; i < frameNum; i++)
        {
            tasks[i].wait();
        }
        fsOut.write(bufOut, frmSzOut * frameNum);
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
            ParseFrameType(frmIn, argv[i] + 3, "Input");
            fsIn.open(argv[++i], std::ios::in | std::ios::binary);
        }
        else if (std::strncmp(argv[i], "-o:", 2) == 0)
        {
            ParseFrameType(frmOut, argv[i] + 3, "Output");
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

    if (!frmIn[0] || !frmOut[0] || !fsIn || !fsOut)
    {
        return -1;
    }

    frame::Frame::EnableLog(true);

    return 0;
}

static void ParseFrameType(frame::Frame** frm, const char* type, const char* name)
{
    std::string tp(type);
    std::transform(tp.begin(), tp.end(), tp.begin(), [](char ch)
        {
            return std::toupper(static_cast<unsigned char>(ch));
        });

    if (tp.empty())
    {
        return;
    }

#define CHECK_TYPE_AND_CREATE(T) else if (#T == tp)  for (size_t i = 0; i < coreNum; i++) frm[i] = CREATE_FRAME(T, w, h, name)

    CHECK_TYPE_AND_CREATE(GREY);
    CHECK_TYPE_AND_CREATE(Y8);
    CHECK_TYPE_AND_CREATE(Y800);
    CHECK_TYPE_AND_CREATE(I400);
    CHECK_TYPE_AND_CREATE(NV12);
    CHECK_TYPE_AND_CREATE(NV21);
    CHECK_TYPE_AND_CREATE(P010);
    CHECK_TYPE_AND_CREATE(Y410);
    CHECK_TYPE_AND_CREATE(Y416);
    CHECK_TYPE_AND_CREATE(AYUV);
    CHECK_TYPE_AND_CREATE(I420);
    CHECK_TYPE_AND_CREATE(Y210);
    CHECK_TYPE_AND_CREATE(Y216);
    CHECK_TYPE_AND_CREATE(YUY2);
    CHECK_TYPE_AND_CREATE(YUYV);

#undef CHECK_TYPE_AND_CREATE
}
