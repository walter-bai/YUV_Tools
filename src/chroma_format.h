enum class CHROMA_FORMAT
{
    YUV_400 = 0,  // no chroma
    YUV_420 = 1,  // half horizontal resolution, half vertical resolution
    YUV_422 = 2,  // half horizontal resolution, full vertical resolution
    YUV_440 = 3,  // full horizontal resolution, half vertical resolution
    YUV_444 = 4   // full horizontal resolution, full vertical resolution
};
