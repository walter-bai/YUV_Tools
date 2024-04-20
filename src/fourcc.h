#define MAKEFOURCC(A, B, C, D) \
  ((((int)A)) + (((int)B) << 8) + (((int)C) << 16) + (((int)D) << 24))

enum class FOURCC
{
    // YUV 400
    I400    = MAKEFOURCC('I', '4', '0', '0'),

    // YUV 420
    I420    = MAKEFOURCC('I', '4', '2', '0'),
    NV12    = MAKEFOURCC('N', 'V', '1', '2'),
    P010    = MAKEFOURCC('P', '0', '1', '0'),
    P012    = MAKEFOURCC('P', '0', '1', '2'),
    P016    = MAKEFOURCC('P', '0', '1', '6'),
    NV21    = MAKEFOURCC('N', 'V', '2', '1'),

    // YUV 422
    I422    = MAKEFOURCC('I', '4', '2', '2'),
    NV16    = MAKEFOURCC('N', 'V', '1', '6'),
    P210    = MAKEFOURCC('P', '2', '1', '0'),
    P216    = MAKEFOURCC('P', '2', '1', '6'),
    YUYV    = MAKEFOURCC('Y', 'U', 'Y', 'V'),  // YUY2
    UYVY    = MAKEFOURCC('U', 'Y', 'V', 'Y'),
    Y210    = MAKEFOURCC('Y', '2', '1', '0'),
    Y216    = MAKEFOURCC('Y', '2', '1', '6'),

    // YUV 444
    I440    = MAKEFOURCC('I', '4', '4', '0'),
    I444    = MAKEFOURCC('I', '4', '4', '4'),
    NV42    = MAKEFOURCC('N', 'V', '4', '2'),
    VUYX    = MAKEFOURCC('V', 'U', 'Y', 'X'),  // AYUV
    Y410    = MAKEFOURCC('Y', '4', '1', '0'),
    Y416    = MAKEFOURCC('Y', '4', '1', '6'),
    NV24    = MAKEFOURCC('N', 'V', '2', '4'),
    P410    = MAKEFOURCC('P', '4', '1', '0'),
    P416    = MAKEFOURCC('P', '4', '1', '6'),

    // RGB
    RGB24   = MAKEFOURCC('R', 'G', 'B', '2'),
    BGR24   = MAKEFOURCC('B', 'G', 'R', '2'),
    X2RGB10 = MAKEFOURCC('X', 'R', 'G', 'B'),
    X2BGR10 = MAKEFOURCC('X', 'B', 'G', 'R'),
    ARGB    = MAKEFOURCC('A', 'R', 'G', 'B'),
    BGRA    = MAKEFOURCC('B', 'G', 'R', 'A'),
    RGBA    = MAKEFOURCC('R', 'G', 'B', 'A'),
    ABGR    = MAKEFOURCC('A', 'B', 'G', 'R'),
    A2RGB10 = MAKEFOURCC('A', '2', 'R', 'G'),
    A2BGR10 = MAKEFOURCC('A', '2', 'B', 'G')
};

#undef MAKEFOURCC
