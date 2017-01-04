#!/bin/bash

mogrify -format rgb images/*.png
g++ skhype.cpp ../process_image_stream/process_image/trans.cpp -o skhype_anim
./skhype_anim > skhype_anim.r16m