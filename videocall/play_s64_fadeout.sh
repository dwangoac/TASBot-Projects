#!/bin/bash

source port_assignments.sh

python3 ./psoc/Scripts/play_r16y_cmd_resync_stdin.py $V_DEVICE < ./supern64_anim/supern64_anim_fadeout.r16m
