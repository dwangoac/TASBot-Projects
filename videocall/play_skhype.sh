#!/bin/bash

source port_assignments.sh

python3 ./psoc/Scripts/play_r16y_cmd_resync_stdin.py $V_DEVICE < ./skhype_anim/skhype_anim.r16m
