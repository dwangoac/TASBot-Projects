wla-6502 -o stage2.asm 
wla-6502 -o stage3.asm 
wla-6502 -o pcm.asm
wla-6502 -o bios.asm
wla-6502 -o credits.asm
wla-6502 -o credits2.asm

wlalink -b stage2.cfg stage2.bin
wlalink -b stage3.cfg stage3.bin
wlalink -b pcm.cfg pcm.bin
wlalink -b bios.cfg bios.bin
wlalink -b credits.cfg credits.bin
wlalink -b credits2.cfg credits2.bin

convert_bin.py 1 2 stage2.bin
reverse_input.py stage2.inp
convert_bin.py 1 2 stage3.bin
convert_bin.py 1 1 pcm.bin
convert_bin.py 1 1 bios.bin
convert_bin.py 1 1 credits.bin
convert_bin.py 1 1 credits2.bin

copy stage2.rev.inp + stage3.inp + credits.inp + credits2.inp + bios.inp + pcm.inp console.inp
create_r16m.py console.inp
copy /b mm_base.r16m + console.r16m build.r16m
