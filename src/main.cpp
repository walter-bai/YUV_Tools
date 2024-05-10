#include <fstream>
#include "frame_converter.hpp"

int main(int argc, char** argv)
{
    converter::FrameConverter<std::ifstream, std::ofstream> cvt;

    return cvt.Execute(argc - 1, &argv[1]);
}
