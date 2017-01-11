#!/bin/bash

source port_assignments.sh

#python3 ./psoc/Scripts/play_r16y_SMB3.py $L_CHANNEL_DEVICE ../games/SMB3_AGDQ/v1/smb3_170110.r16m
#python3 ./psoc/Scripts/play_r16y_SMB3.py $L_CHANNEL_DEVICE ../games/SMB3_AGDQ/v2/smb3_pcm.r16m
python3 ./psoc/Scripts/play_r16y_SMB3.py $L_CHANNEL_DEVICE ../games/SMB3_AGDQ/v3/smb3_pcm.r16m
