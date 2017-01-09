#!/bin/bash

V_FILE=$1
ffmpeg -y -i "$V_FILE" -ar 54235 -map_channel 0.1.0 -f s16le /tmp/aaaL -map_channel 0.1.1 -ar 54235 -f s16le /tmp/aaaR -map 0:v -r 295312500/30465281 -vf scale=128:112 -f rawvideo -pix_fmt rgb24 /tmp/vvv
