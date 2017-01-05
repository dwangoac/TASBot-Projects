#!/bin/bash

mogrify -format rgb images/*.png
g++ supern64.cpp ../process_image_stream/process_image/trans.cpp -o supern64_anim
./supern64_anim > supern64_anim.r16m