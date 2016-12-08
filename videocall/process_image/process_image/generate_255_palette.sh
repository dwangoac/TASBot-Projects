#!/bin/bash

convert $1 -colors 255 -unique-colors -depth 8 -alpha off txt:- > palette.txt
perl process_255_palette.pl > palette.bin
rm palette.txt