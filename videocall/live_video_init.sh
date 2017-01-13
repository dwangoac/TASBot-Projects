#!/bin/bash

if ! [ -e /tmp/aaaL ] || ! [ -p /tmp/aaaL ]; then
  if [ -e /tmp/aaaL ]; then
    rm /tmp/aaaL;
  fi;
  mkfifo /tmp/aaaL;
fi;

if ! [ -e /tmp/aaaR ] || ! [ -p /tmp/aaaR ]; then
  if [ -e /tmp/aaaR ]; then
    rm /tmp/aaaR;
  fi;
  mkfifo /tmp/aaaR;
fi;

if ! [ -e /tmp/vvv ] || ! [ -p /tmp/v ]; then
  if [ -e /tmp/vvv ]; then
    rm /tmp/vvv;
  fi;
  mkfifo /tmp/vvv;
fi;

source port_assignments.sh

python3 ./psoc/Scripts/recover.py $L_CHANNEL_DEVICE
python3 ./psoc/Scripts/recover.py $R_CHANNEL_DEVICE

(./splitaudio2 /dev/stdout mm /run/user/1001/timing.tmp < /tmp/aaaL | ./pipebuf 10000 | python3 ./psoc/Scripts/play_r16y.py $L_CHANNEL_DEVICE /dev/stdin) &
(./splitaudio2 /dev/stdout mm /run/user/1001/timing2.tmp < /tmp/aaaR | ./pipebuf 10000 | python3 ./psoc/Scripts/play_r16y.py $R_CHANNEL_DEVICE /dev/stdin) &
(./process_image_stream/process_image/transout_full 5 < /tmp/vvv | python3 ./psoc/Scripts/play_r16y_cmd_resync_stdin.py $V_DEVICE)
