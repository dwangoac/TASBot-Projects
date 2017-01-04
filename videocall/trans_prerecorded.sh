#!/bin/bash

frame_skip=$2

# calculate framerate to convert video to and output frames
  # (60 * X) / ((6 * x) + 1)
framerate_num=$((60 * $frame_skip))
framerate_den=$(((6 * $frame_skip) + 1))

g++ -O2 process_image_stream/process_image/transout_full.cpp process_image_stream/process_image/octree.cpp process_image_stream/process_image/trans.cpp -o process_image_stream/process_image/transout_full
ffmpeg -y -i "$1" -r $framerate_num/$framerate_den -vf scale=128:112 -f rawvideo -pix_fmt rgb24 /dev/stdout | ./process_image_stream/process_image/transout_full $frame_skip > out.r16m
cat vphone-v3b-tompa-loz-v2.latch.r16m process_image_stream/process_image/base_x300_borderless.bin out.r16m > LoZ_out.r16m

