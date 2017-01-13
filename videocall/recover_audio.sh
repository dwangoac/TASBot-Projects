#!/bin/bash

source port_assignments.sh

python3 psoc/Scripts/recover.py $L_CHANNEL_DEVICE
python3 psoc/Scripts/recover.py $L_CHANNEL_DEVICE
python3 psoc/Scripts/recover.py $R_CHANNEL_DEVICE
python3 psoc/Scripts/recover.py $R_CHANNEL_DEVICE
