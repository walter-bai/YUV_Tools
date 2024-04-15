# YUV_Tools

Convert one YUV format to another

yuv_tools -w 1920 -h 1080 -i:y410 D:\yuv\Blender_lossless_y410_1080p60.yuv -o:nv12 \\10.239.141.175\content-share\EncoderContent\Blender_lossless_nv12_1080p60.yuv

Add "-p" or "--padding" to make alignment and padding, e.g., "-p 8" will align both width and height to 8, the alignmed pixels will be padded by boundary replication.