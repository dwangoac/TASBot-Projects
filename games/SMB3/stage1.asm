.include "rommap.i"
.org $91

l1:                 ; Shellcode (koopa shell) loader
	jsr $febe       ; This loader is what is created by killing and placing
    sta $9e         ; the koopas and their shells in specific spots.
    brk             ; It's very simple self-modifying code that writes data
    brk             ; into $9e-$ff and then continues execution at $9e
    brk
    brk				
    inc $95
    bmi l1