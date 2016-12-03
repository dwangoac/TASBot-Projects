#!/bin/bash

convert $1 -crop 4x2@ +repage +adjoin -colors 15 -unique-colors -depth 8 txt:- > palettes.txt
perl process_8_palettes.pl > palettes.bin
rm palettes.txt