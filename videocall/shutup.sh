#!/bin/bash

source port_assignments.sh

dd if=/dev/zero bs=16 count=4096 | python3 ./psoc/Scripts/play_r16y.py $L_CHANNEL_DEVICE /dev/stdin
dd if=/dev/zero bs=16 count=4096 | python3 ./psoc/Scripts/play_r16y.py $R_CHANNEL_DEVICE /dev/stdin
