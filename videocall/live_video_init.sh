#!/bin/bash

R_CHANNEL_DEVICE=/dev/ttyACM0
L_CHANNEL_DEVICE=/dev/ttyACM1
V_DEVICE=/dev/ttyACM2

(./splitaudio2 /dev/stdout mm /run/user/1001/timing.tmp < /tmp/aaaL | ./pipebuf 10000 | python3 ./psoc/Scripts/play_r16y.py $L_CHANNEL_DEVICE /dev/stdin) &
(./splitaudio2 /dev/stdout mm /run/user/1001/timing2.tmp < /tmp/aaaR | ./pipebuf 10000 | python3 ./psoc/Scripts/play_r16y.py $R_CHANNEL_DEVICE /dev/stdin) &
(./process_image_stream/process_image/transout_full 5 < /tmp/vvv | python3 ./psoc/Scripts/play_r16y_cmd_resync_stdin.py $V_CHANNEL_DEVICE)
