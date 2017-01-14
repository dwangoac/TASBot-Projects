#!/bin/bash

ffmpeg -video_size 128x112 -f x11grab -i :0.0+1486,837 -y -r 295312500/30465281 -f rawvideo -pix_fmt rgb24 /tmp/vvv