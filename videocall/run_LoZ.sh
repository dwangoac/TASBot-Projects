#!/bin/bash

source port_assignments.sh

cat vphone-v3e-tompa-loz.latch.r16m supern64_anim/supern64_anim.r16m > LoZ_out.r16m
python3 ./psoc/Scripts/play_r16y_cmd_init.py $V_DEVICE LoZ_out.r16m