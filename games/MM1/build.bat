wla-6502 -o stage2.asm 
wla-6502 -o stage3.asm 
wla-6502 -o pcm.asm
wla-6502 -o bios.asm
wlalink -b stage2.cfg stage2.bin
wlalink -b stage3.cfg stage3.bin
wlalink -b pcm.cfg pcm.bin
wlalink -b bios.cfg bios.bin

convert_bin.py 1 2 stage2.bin
reverse_input.py stage2.inp
convert_bin.py 1 2 stage3.bin
convert_bin.py 1 1 pcm.bin
convert_bin.py 1 1 bios.bin

copy stage2.rev.inp + stage3.inp + bios.inp + pcm.inp console.inp
create_r16m.py console.inp
copy /b mm_base.r16m + console.r16m build.r16m
