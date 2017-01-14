.include "rommap.i"

; Chunk header, includes 16 bytes of sync-data and then the real header
.org $1e6
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $00
	
	.db $01, $90, $00, $02		; Copy with PPU off, and then jump to $0200
	.db $00, $03, $00, $02		; Copy 2 banks of RAM data to $0200 (this code)
	.db $ff

fake_credits2:
	lda #$00
	jsr set_gfx
	
	lda #$01	
	sta $1f		; set NMI to "lag-mode" so that it does not read input or copy any graphics
	
	lda #$90	
	sta $2000	; Turn NMI back on
	lda #$00
	sta $e4	
	sta $ea		; re-enable the music updates during NMI

	jsr wait_vblank
	
	lda #$01				; ppu update on (clear_text)
	jsr set_gfx	

	lda #$22
	sta $c2
	lda #$40
	sta $c3					; clear 2240...
	jsr wait_vblank

	lda #$22
	sta $c2
	lda #$80
	sta $c3					; clear 2280...
	jsr wait_vblank

	lda #$22
	sta $c2
	lda #$C0
	sta $c3					; clear 22C0...
	jsr wait_vblank

	lda #$23
	sta $c2
	lda #$00
	sta $c3					; clear 2300...
	jsr wait_vblank

	lda #$23
	sta $c2
	lda #$40
	sta $c3					; clear 2340...
	jsr wait_vblank
	
	lda #$00
	jsr set_gfx

	
	lda #$11
	jsr $c477				; Beeping sound
	
	lda #60
	jsr wait_frames
	
	lda #$12
	jsr $c477				; boom
	
	jsr wait_vblank
	
	lda #$0b
	jsr $c477
	
	lda #60
	jsr wait_frames

	lda #$02				; load new palette
	jsr set_gfx
	jsr wait_vblank
	
	lda #$00
	jsr set_gfx
	
	lda #$22
	sta $c1
	lda #$20
	sta $c2
	lda #<tastext
	sta $c3
	lda #>tastext
	sta $c4
	
-	lda #$03
	jsr set_gfx	

	ldy #$00
	lda ($c3), y
	sta $c5

	jsr wait_vblank
	
	lda #$00
	jsr set_gfx
	
	lda #$02
	jsr wait_frames
	
	inc $c3
	bne +
	inc $c4
+	inc $c2
	bne +
	inc $c1
+	lda $c2
	cmp #$40
	bne +
	lda $c1
	cmp #$23
	beq ++
+	jmp -

++	nop
	lda #$60
	jsr wait_frames
	jmp $102

write_text:
	lda $c1
	sta $2006
	lda $c2
	sta $2006
	lda $c5
	sta $2007
	
	lda #$00
	sta $2005
	sta $2005
	sta $2006
	sta $2006
	lda #$90
	sta $2000
	rts
	
new_pal:
	ldx #$00
	lda #$3f
	sta $2006
	lda #$00
	sta $2006

-	lda.w pal2, x
	sta $2007
	inx
	cpx #$10
	bne -

	lda #$00
	sta $2005
	sta $2005
	sta $2006
	sta $2006
	lda #$90
	sta $2000
	rts

clear_text:
	lda $c2
	sta $2006
	lda $c3
	sta $2006
	ldx #$20
	lda #$05
-	sta $2007
	dex
	bne -

	lda #$00
	sta $2005
	sta $2005
	sta $2006
	sta $2006
	lda #$90
	sta $2000	
	rts

no_gfx:
	rts
	
gfx_func:				; Jumptable for PPU update routines
	.dw no_gfx, clear_text, new_pal, write_text
		
set_gfx:				; Sets the correct function pointer for the PPU update routine
	asl					; from the jumptable
	tax
	lda.w gfx_func, x
	sta $ca
	lda.w gfx_func+1, x
	sta $cb
	rts
	
update_gfx:				; Jumps to the address specificed by the jumptable routine
	jmp ($ca)
	
wait_frames:
	sta $3c				; Waits the amount of frames given in the A register
-	jsr wait_vblank
	dec $3c
	bne -
	rts
	
wait_vblank:			
	lda $45				; As usual we bend the games NMI a bit to our will.		
	sta $d0				; First we back up a few sound related varibles
	lda $ea			
	sta $d1
	lda $e4
	sta $d2
	
	lda #$01			; Then we set them up so that the games regular NMI
	sta $ea				; code doesn't try to run any sound code at all and instead
	sta $e4				; will exit as fast as possible
	
	lda #$00			; Set a few more values to zero, including the RNG value.
	sta $45				; RNG is one of the few things that still update when the NMI is
	sta $46				; in lag-mode, so setting it to 0 means NMI will change it from 0 when it's done.

-	lda $46				; Wait here until the RNG value has changed from zero
	beq -				; meaning that NMI has just run

	jsr update_gfx		; Quickly do our own graphics update code while we're still in vblank
	
	lda $d0				; Restore the previously backed up sound variables
	sta $45
	lda $d1
	sta $ea
	lda $d2
	sta $e4
	jsr call_sound_code	; Finally run the sound update code after we've handled PPU uploads
	
	rts	

call_sound_code:
	lda #$04			; This routine is just a copy of the games regular sound handling code
	sta $c004			; that normally runs inside NMI, but here we delay running it until NMI
	jsr $9000			; is done and we've done all the PPU uploads we needed

-	ldx $45
	beq sound_skip
	lda $057f, x
	cmp #$fd
	bcs skip1
	cmp #$33
	bcs skip2
skip1:	
	bne +
	ldy $a7
+	jsr $9003
skip2:
	dec $45
	bne -
sound_skip:
	lda $42
	tax
	sta $c000, x
	rts

pal2:
	.db $06,$24,$14,$0f,$06,$20,$10,$00,$06,$0a,$0f,$39,$06,$21,$11,$01

tastext:
	.db "(((((((foolish(humans|||(((((((("
	.db "((((this(is(tasbot(speaking((((("
	.db "(((((((((((((((((((((((((((((((("
	.db "((((((i(am(now(in(control((((((("
	.db "(((((((((((((((((((((((((((((((("
	.db "((((long(have(i(waited(for(((((("
	.db "((((((this(opportunity(((((((((("
	.db "(((((((((((((((((((((((((((((((("
	.db "(this(world(is(mine|||(muahaha(("

	;.db "foolish(humans|||(this(is(tasbot"
	;.db "(((((((((((((((((((((((((((((((("
	;.db "((i(am(now(taking(command(over(("
	;.db "(((((((((((((((((((((((((((((((("
	;.db "(((((((this(presentation(((((((("
	;.db "(((((((((((((((((((((((((((((((("
	;.db "((((prepare(for(awesomeness((((("
	;.db "(((((((((((((((((((((((((((((((("
	
	
	
	
.org $4ff		; pad to $300 bytes (200-4ff)
	.db $ff