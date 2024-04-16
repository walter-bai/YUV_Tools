# YUV_Tools
This tool converts a YUV from one format to another, even the source and destination format have different bit depth and chroma format. It can also align the width and height with user-specified alignment and padding method.

## Build
`cd repo`
`mkdir build && cd build`
`cmake ..`

## Options
- [-w|--width] pixel width of input YUV
- [-h|--height] pixel height of input YUV
- [-i:format] format of input YUV
- [-o:format] format of output YUV
- [-a|--align] width and height alignment for the output YUV, must be an even number, output YUV will be padded if width or height is not aligned
- [-r|--replicate] padding method, 0 for zero padding, 1 for boundary replication padding

## Example
The following cmdline converts a Y410 file to an NV12 one without padding:
`yuv_tools -w 1920 -h 1080 -i:y410 input.y410 -o:nv12 output.nv12`
