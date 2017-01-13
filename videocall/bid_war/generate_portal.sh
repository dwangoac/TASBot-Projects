#!/bin/bash

mogrify -format rgb images/*.png
g++ portal.cpp ../process_image_stream/process_image/trans.cpp -o portal_anim
./portal_anim > portal_anim.r16m