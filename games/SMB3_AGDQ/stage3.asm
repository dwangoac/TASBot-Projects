.include "rommap.i"
.org $102
					
l3:					; Third stage loader, loads data into $6000
	nop				; Much more advanced than Stage 2 since it will first
	nop             ; sync to the input stream using extra 0xFF's.
	nop             ; It loads at a higer data rate, and it has no
	nop             ; restrictions on what bits can be used anymore.
	nop             ; Jumps to $6000 when loading is complete
	nop
	nop
	nop
	sei             ; ignore IRQs
	cld             ; disable decimal mode
	ldx #$40
	stx $4017       ; disable APU frame IRQ
	ldx #$00
	stx $2001       ; disable rendering
	stx $4010       ; disable DMC IRQs
	ldy #$00
skip:
    jsr $ff12       ; Call the games controller reading routine and read
	lda $00         ; data until we are not reading 0xFF, this is to sync
	cmp #$ff        ; the input stream so we know exactly what data we
	beq skip        ; are reading below.
	lda #$00
	sta $10
	lda #$60
	sta $11
load:
    ldy #$00        ; Below here is just a routine that loads data from 
	jsr $ff12       ; controller 1 and puts it into $6000 and up.
	lda $00
	sta ($10),y     ; Right now the amount copied is hardcoded into this
	lda $10         ; routine and can only be specified to multiples of
	cmp #$ff        ; 256
	bne noinc
	inc $11
noinc:
    and #%1110001
	sta $2001
	inc $10
	lda $11
	cmp #$80        ; Keep copying until target address is $8000
	bne load
	jmp $6000
	
	.db $FF,$FF,$FF         ; Pad to fill to 17f
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF
	.db $FF,$FF,$FF