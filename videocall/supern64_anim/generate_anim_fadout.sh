#!/bin/bash

mogrify -format rgb images/*.png
g++ supern64_fadeout.cpp ../process_image_stream/process_image/trans.cpp -o supern64_anim_fadeout
./supern64_anim_fadeout > supern64_anim_fadeout.r16m