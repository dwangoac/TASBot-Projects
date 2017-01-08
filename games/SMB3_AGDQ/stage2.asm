.include "rommap.i"
.org $9A            
	
	nop				; Stage 2 loader (written by koopacode)
	nop             ; Cannot contain bytes that would use
	nop             ; up/down or left/right
	nop             ; All the nops are inserted because of NMI randomly
	nop             ; stealing inputs when it occurs during stage 1 loading
	nop             ; that we cannot stop.
	sei
	tya
	sta $2000,y     ; disable NMI
	sta $4015,y     ; stop DPCM audio
	ldx #$fa        ; Adjust stack
	txs
	nop
	nop
	nop
	nop             ; Magic NOP sled yay!
	nop
	nop
	nop
	nop
	lda #$60        ; This rewrites part of the stage1 loader and adds
	sta $94         ; and RTS after the controller reading part, so that
	ldy #$01        ; we can call it from this routine.
	sty $f1
	iny
	sty $f0         ; Set up variables for the indirect write to $102
	nop
l2:
    nop
	nop
	nop
	nop
	nop             ; NOP's are the best
	nop
	nop
	nop
	nop
	nop
	nop
	jsr $0091       ; Get controller data and save it into $102-$17f
	lda $00
	sta ($f0),y
	inc $f0
	bpl l2          ; Branch if $f0 is less than $80
	jsr $102        ; Jumps to $102 and into the stage3 loader

.org $F0			; Pad payload with NOP's at end so a slight mismatch in alignment can still work
	.db $ea, $ea, $ea, $ea, $ea, $ea, $ea, $ea, $ea, $ea, $ea, $ea, $ea, $ea, $ea, $ea