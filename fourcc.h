#define MAKEFOURCC(A, B, C, D) \
  ((((int)A)) + (((int)B) << 8) + (((int)C) << 16) + (((int)D) << 24))

enum class FOURCC
{
	UNKNOWN    = 0,
	NV12       = MAKEFOURCC('N', 'V', '1', '2'),
	YV12       = MAKEFOURCC('Y', 'V', '1', '2'),
	NV16       = MAKEFOURCC('N', 'V', '1', '6'),
	YUY2       = MAKEFOURCC('Y', 'U', 'Y', '2'),
	RGB565     = MAKEFOURCC('R', 'G', 'B', '2'),
	RGBP       = MAKEFOURCC('R', 'G', 'B', 'P'),
	RGB4       = MAKEFOURCC('R', 'G', 'B', '4'),
	P8         = 41,
	P8_TEXTURE = MAKEFOURCC('P', '8', 'M', 'B'),
	P010       = MAKEFOURCC('P', '0', '1', '0'),
	P016       = MAKEFOURCC('P', '0', '1', '6'),
	P210       = MAKEFOURCC('P', '2', '1', '0'),
	BGR4       = MAKEFOURCC('B', 'G', 'R', '4'),
	A2RGB10    = MAKEFOURCC('R', 'G', '1', '0'),
	ARGB16     = MAKEFOURCC('R', 'G', '1', '6'),
	ABGR16     = MAKEFOURCC('B', 'G', '1', '6'),
	R16        = MAKEFOURCC('R', '1', '6', 'U'),
	AYUV       = MAKEFOURCC('A', 'Y', 'U', 'V'),
	AYUV_RGB4  = MAKEFOURCC('A', 'V', 'U', 'Y'),
	UYVY       = MAKEFOURCC('U', 'Y', 'V', 'Y'),
	Y210       = MAKEFOURCC('Y', '2', '1', '0'),
	Y410       = MAKEFOURCC('Y', '4', '1', '0'),
	Y216       = MAKEFOURCC('Y', '2', '1', '6'),
	Y416       = MAKEFOURCC('Y', '4', '1', '6'),
	NV21       = MAKEFOURCC('N', 'V', '2', '1'),
	IYUV       = MAKEFOURCC('I', 'Y', 'U', 'V'),
	I010       = MAKEFOURCC('I', '0', '1', '0'),
	I210       = MAKEFOURCC('I', '2', '1', '0'),
	I420       = IYUV,
	I422       = MAKEFOURCC('I', '4', '2', '2'),
	BGRA       = RGB4,
	BGRP       = MAKEFOURCC('B', 'G', 'R', 'P'),
	XYUV       = MAKEFOURCC('X', 'Y', 'U', 'V'),
	ABGR16F    = MAKEFOURCC('B', 'G', 'R', 'F')
};

#undef MAKEFOURCC
