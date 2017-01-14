#!/bin/bash

mogrify -format rgb images/*.png
g++ twitch.cpp ../process_image_stream/process_image/trans.cpp -o twitch_anim
./twitch_anim > twitch_anim.r16m