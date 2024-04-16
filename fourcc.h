// Reference:
// https://github.com/intel/vpl-gpu-rt/blob/main/api/vpl/mfxstructures.h
// https://fourcc.org/yuv.php

#define MAKEFOURCC(A, B, C, D) \
  ((((int)A)) + (((int)B) << 8) + (((int)C) << 16) + (((int)D) << 24))

enum class FOURCC
{
    UNKNOWN    = 0,

    // YUV 400
    GREY       = MAKEFOURCC('G', 'R', 'E', 'Y'),
    Y8         = MAKEFOURCC('Y', '8', ' ', ' '),
    Y800       = MAKEFOURCC('Y', '8', '0', '0'),

    // YUV 420
    NV12       = MAKEFOURCC('N', 'V', '1', '2'),
    NV16       = MAKEFOURCC('N', 'V', '1', '6'),
    I420       = MAKEFOURCC('I', '4', '2', '0'),
    P010       = MAKEFOURCC('P', '0', '1', '0'),
    P016       = MAKEFOURCC('P', '0', '1', '6'),
    NV21       = MAKEFOURCC('N', 'V', '2', '1'),
    IYUV       = MAKEFOURCC('I', 'Y', 'U', 'V'),
    I010       = MAKEFOURCC('I', '0', '1', '0'),
    YV12       = MAKEFOURCC('Y', 'V', '1', '2'),

    // YUV 422
    YUY2       = MAKEFOURCC('Y', 'U', 'Y', '2'),
    YUYV       = MAKEFOURCC('Y', 'U', 'Y', 'V'),
    I422       = MAKEFOURCC('I', '4', '2', '2'),
    P210       = MAKEFOURCC('P', '2', '1', '0'),
    Y210       = MAKEFOURCC('Y', '2', '1', '0'),
    UYVY       = MAKEFOURCC('U', 'Y', 'V', 'Y'),
    Y216       = MAKEFOURCC('Y', '2', '1', '6'),
    I210       = MAKEFOURCC('I', '2', '1', '0'),

    // YUV 444
    AYUV       = MAKEFOURCC('A', 'Y', 'U', 'V'),
    Y410       = MAKEFOURCC('Y', '4', '1', '0'),
    Y416       = MAKEFOURCC('Y', '4', '1', '6'),
    XYUV       = MAKEFOURCC('X', 'Y', 'U', 'V'),

    // RGB
    RGB565     = MAKEFOURCC('R', 'G', 'B', '2'),
    RGBP       = MAKEFOURCC('R', 'G', 'B', 'P'),
    RGB4       = MAKEFOURCC('R', 'G', 'B', '4'),
    BGR4       = MAKEFOURCC('B', 'G', 'R', '4'),
    A2RGB10    = MAKEFOURCC('R', 'G', '1', '0'),
    ARGB16     = MAKEFOURCC('R', 'G', '1', '6'),
    ABGR16     = MAKEFOURCC('B', 'G', '1', '6'),
    ABGR16F    = MAKEFOURCC('B', 'G', 'R', 'F'),
    BGRA       = MAKEFOURCC('B', 'G', 'R', 'A'),
    BGRP       = MAKEFOURCC('B', 'G', 'R', 'P'),

    R16        = MAKEFOURCC('R', '1', '6', 'U')
};

#undef MAKEFOURCC
