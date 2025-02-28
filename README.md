# YUV_Tools
This tool converts a YUV from one format to another, even the source and destination have different bit depth and chroma format. It can also align the width and height with user-specified alignment and padding method.

## Build
* `cd repo`
* `mkdir build && cd build`
* `cmake ..` or `cmake -DENABLE_TESTS=ON ..`

## Options
- [-w|--width] pixel width of input YUV
- [-h|--height] pixel height of input YUV
- [-i:format] format of input YUV
- [-o:format] format of output YUV
- [-a|--align] width and height alignment for the output YUV, must be an even number, output YUV will be padded if width or height is not aligned
- [-r|--replicate] padding method, 0 for zero padding, 1 for boundary replication padding
- [-n] number of frames
- [-n:beg] start frame index, 0 to number of frames in YUV file minus 1, inclusive
- [-n:end] end frame index, 0 to number of frames in YUV file minus 1, inclusive, end must >= beg

## Example
* Convert a Y410 file to an NV12 one without padding:  
`yuv_tools -w 1920 -h 1080 -i:y410 input.y410 -o:nv12 output.nv12`
* Convert a Y210 file to an Y410 one with 16 aligned using zero padding:  
`yuv_tools -w 1920 -h 1080 -i:y210 input.y210 -o:y410 output.y410 -a 16`
* Align an I420 file against 32 using boundary replication padding:  
`yuv_tools -w 1920 -h 1080 -i:i420 input.yuv -o:i420 output.yuv -a 32 - r 1`
* Extract frame 2 to frame 5 of a P010 file  
`yuv_tools -w 1920 -h 1080 -i:p010 input.yuv -o:p010 output.yuv -n:beg 2 -n:end 5`
* Convert first 10 frames of a P010 file to NV12  
`yuv_tools -w 1920 -h 1080 -i:p010 input.yuv -o:nv12 output.yuv -n 10`
* Convert 10 frames of a AYUV file to YUY2, starting from frame 7  
`yuv_tools -w 1920 -h 1080 -i:ayuv input.yuv -o:yuy2 output.yuv -n 10 -n:beg 7`