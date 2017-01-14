#!/bin/bash

ffmpeg -y -f alsa -i loopout -ac 2 -ar 54235 -map_channel 0.0.0 -f s16le /tmp/aaaL -map_channel 0.0.1 -ar 54235 -f s16le /tmp/aaaR