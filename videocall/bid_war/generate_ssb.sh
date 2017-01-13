#!/bin/bash

mogrify -format rgb images/*.png
g++ ssb.cpp ../process_image_stream/process_image/trans.cpp -o ssb_anim
./ssb_anim > ssb_anim.r16m