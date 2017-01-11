wla-6502 stage2.asm 
wla-6502 stage3.asm 
wla-6502 payload.asm

wlalink -b stage2.cfg stage2.o
wlalink -b stage3.cfg stage3.o
wlalink -b payload.cfg payload.o

python3 convert_bin_unix.py 4 stage2.o
python3 convert_bin_unix.py 4 stage3.o
python3 convert_bin_unix.py 1 payload.o

cat base_glitch.txt stage2.inp stage3.inp payload.inp > console.inp
python3 create_r16m_unix.py console.inp
cat smb3_base.r16m console.r16m > build.r16m
