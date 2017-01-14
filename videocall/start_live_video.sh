#!/bin/bash

ffmpeg -video_size 640x560 -f x11grab -i :0.0+973,53 -y -r 295312500/30465281 -vf scale=128:112 -f rawvideo -pix_fmt rgb24 /tmp/vvv