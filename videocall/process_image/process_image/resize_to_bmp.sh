#!/bin/bash

convert $1 -resize $3x$4^ -gravity center -extent $3x$4 -type TrueColor -depth 8 BMP:$2