cd famitone2
call build
cd ..

wla-6502 -o stage2.asm 
wla-6502 -o stage3.asm 
wla-6502 -o payload.asm
wlalink -b stage2.cfg stage2.bin
wlalink -b stage3.cfg stage3.bin
wlalink -b payload.cfg payload.bin

convert_bin.py 4 stage2.bin
convert_bin.py 4 stage3.bin
convert_bin.py 1 payload.bin

copy base_glitch.txt + stage2.inp + stage3.inp + payload.inp console.inp
create_r16m.py console.inp
copy /b smb3_base.r16m + console.r16m build.r16m
