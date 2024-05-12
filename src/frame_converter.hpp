#include <algorithm>
#include <cstring>
#include <future>
#include <iostream>
#include <thread>
#include "frame.hpp"
#include "fourcc.h"

namespace converter
{
    template <typename IStream, typename OStream>
    class FrameConverter final
    {
    public:
        FrameConverter() = default;

        int Execute(int argc, const char* const * argv)
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

            if (!fsIn.seekg(std::ios_base::beg + frmSzIn * beg))
            {
                return -1;
            }

            size_t frmNum2Read = std::min(coreNum, end - beg + 1);
            while (fsIn.read(bufIn, frmSzIn * frmNum2Read), fsIn.gcount())
            {
                size_t frmNumRead = std::min(fsIn.gcount() / frmSzIn, frmNum2Read);
                std::vector<std::future<void>> tasks(frmNumRead);
                for (size_t i = 0; i < frmNumRead; i++)
                {
                    tasks[i] = std::async(
                        std::launch::async,
                        [=]() {
                            frmIn[i]->ReadFrame(bufIn + frmSzIn * i);
                            frmOut[i]->ConvertFrom(*frmIn[i]);
                            frmOut[i]->WriteFrame(bufOut + frmSzOut * i);
                        });
                }
                for (size_t i = 0; i < frmNumRead; i++)
                {
                    tasks[i].wait();
                }
                fsOut.write(bufOut, frmSzOut * frmNumRead);
                beg += frmNumRead;
                frmNum2Read = std::min(coreNum, end - beg + 1);
            }

            return 0;
        }

    private:
        void ParseFrameType(frame::Frame** frm, const char* type, const char* name)
        {
            std::string tp(type);
            std::transform(tp.begin(), tp.end(), tp.begin(), [](char ch)
                {
                    return std::toupper(static_cast<unsigned char>(ch));
                });

            using namespace frame;
#define CHECK_TYPE_AND_CREATE(T) else if (#T == tp)  for (size_t i = 0; i < coreNum; i++) frm[i] = CREATE_FRAME(T, w, h, name)

            if (tp.empty())
            {
                return;
            }
            CHECK_TYPE_AND_CREATE(I400);
            CHECK_TYPE_AND_CREATE(I420);
            CHECK_TYPE_AND_CREATE(NV12);
            CHECK_TYPE_AND_CREATE(P010);
            CHECK_TYPE_AND_CREATE(P012);
            CHECK_TYPE_AND_CREATE(P016);
            CHECK_TYPE_AND_CREATE(NV21);
            CHECK_TYPE_AND_CREATE(I422);
            CHECK_TYPE_AND_CREATE(NV16);
            CHECK_TYPE_AND_CREATE(P210);
            CHECK_TYPE_AND_CREATE(P216);
            CHECK_TYPE_AND_CREATE(YUYV);
            CHECK_TYPE_AND_CREATE(YUY2);
            CHECK_TYPE_AND_CREATE(UYVY);
            CHECK_TYPE_AND_CREATE(Y210);
            CHECK_TYPE_AND_CREATE(Y216);
            CHECK_TYPE_AND_CREATE(I440);
            CHECK_TYPE_AND_CREATE(I444);
            CHECK_TYPE_AND_CREATE(NV42);
            CHECK_TYPE_AND_CREATE(VUYX);
            CHECK_TYPE_AND_CREATE(AYUV);
            CHECK_TYPE_AND_CREATE(Y410);
            CHECK_TYPE_AND_CREATE(Y416);
            CHECK_TYPE_AND_CREATE(NV24);
            CHECK_TYPE_AND_CREATE(P410);
            CHECK_TYPE_AND_CREATE(P416);

#undef CHECK_TYPE_AND_CREATE
        }

        int ParseArgs(int argc, const char* const * argv)
        {
            size_t n = -1;
            for (auto i = 0; i < argc; ++i)
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
                else if (std::strcmp(argv[i], "-n:beg") == 0)
                {
                    beg = strtoull(argv[++i], nullptr, 10);
                }
                else if (std::strcmp(argv[i], "-n:end") == 0)
                {
                    end = strtoull(argv[++i], nullptr, 10);
                }
                else if (std::strcmp(argv[i], "-n") == 0)
                {
                    n = strtoull(argv[++i], nullptr, 10);
                }
            }

            if (!frmIn[0] || !frmOut[0] || !fsIn || !fsOut || beg > end ||
                n == 0 || (end != -2 && n != -1))
            {
                return -1;
            }

            if (n != -1)
            {
                end = beg + n - 1;
            }

            frame::Frame::EnableLog(true);

            return 0;
        }

    private:
        const size_t coreNum = std::thread::hardware_concurrency();
        size_t w = 0;
        size_t h = 0;
        frame::Frame** frmIn = nullptr;
        frame::Frame** frmOut = nullptr;
        IStream fsIn;
        OStream fsOut;
        size_t alignment = 2;
        bool replicate = false;
        size_t beg = 0;
        size_t end = -2;
    };
}
