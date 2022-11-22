#include <fstream>
#include <iostream>
#include "frame.h"

int main(int /*argc*/, char* argv[])
{
    std::ifstream ifs(argv[1], std::ios::in | std::ios::binary);
    std::ofstream ofs("\\\\10.239.141.175\\content-share\\EncoderContent\\Blender_lossless_p010_1080p60.yuv", std::ios::out | std::ios::binary);
    frame::Y410 frm(1920, 1080);
    size_t szIn = frm.FrameSize();
    size_t szOut = 1920 * 1080 * 3;
    auto bufIn = new char[szIn];
    auto bufOut = new char[szOut];

    while (ifs.read(bufIn, szIn))
    {
        frm.ReadFrame(bufIn);
        frm.WriteFrameP010(bufOut);
        ofs.write(bufOut, szOut);
    }

    return 0;
}
