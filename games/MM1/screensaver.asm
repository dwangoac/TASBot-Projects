.include "rommap.i"

; Chunk header, includes 16 bytes of sync-data and then the real header
.org $2de
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $00
	
	.db $01, $80, $00, $03		; Copy with PPU on, set $2000 to $80 while copying, and then jump to $0300
	.db $00, $05, $00, $03		; Copy 2 banks of RAM data to $0300 (this code)
	.db $01, $04, $00, $24		; Copy 4 banks of PPU data to $2400 (nametable + attributes)
	.db $01, $10, $00, $10		; Copy 16 banks of PPU data to $1000 (one bank of CHR data)
	.db $ff						; End of payload

.org $300
payload:
	sei        ; ignore IRQs
	cld        ; disable decimal mode
	ldx #$40
	stx $4017  ; disable APU frame IRQ
	ldx #$ff
	txs        ; Set up stack
	inx        ; now X = 0
	stx $4010  ; disable DMC IRQs	
	stx $4015  ; disable all soundchannels

	stx $2000  ; disable NMI
	stx $2001  ; disable rendering

	lda #$01
	sta $1f
	sta $ea
	sta $e4

	; Initialize variables to zero
	lda #$00
	sta $80
	sta $81
	sta $45
	sta $46
	sta $ca
	sta $cb
	sta $3c
	sta $cf
	jsr set_gfx

	; Load palette data
	ldx #$00
	lda #$3f
	sta $2006
	lda #$00
	sta $2006
	
-	lda.w paldata,x
	sta $2007
	inx
	cpx #$10
	bne -

	ldx #$00
	lda #$3f
	sta $2006
	lda #$10
	sta $2006

-	lda.w paldata,x
	sta $2007
	inx
	cpx #$10
	bne -

	; Clear sprite memory area
	lda #$00
	tax
-	sta $200, x
	sta $700, x
	inx
	bne -

	jsr init_sprites
	jsr update_sprites
	
	; turn PPU back on here
	lda #%00011010
	sta $2001      

	lda #$00
	sta $2005
	sta $2005
	sta $2006
	sta $2006
	lda #$99
	sta $2000
	sta $ff	
	
	lda #$01
	jsr set_gfx
	
	lda #$00
	sta $c0
	sta $c1
	
main:
	jsr wait_vblank
	jsr update_sprites
	
	inc $c0
	bne +
	inc $c1
	lda $c1
	cmp #$0a
	beq ++
+	jmp main
++	jmp $0102

update_sprites:
	ldx #$00
	ldy #$00
start_move:
	lda $702, x
	beq move_left
move_right:
	lda $700, x
	clc
	adc #$01		; update left pos of first column
	sta $700, x
	sta $203, y		; move all 16 sprites left or right
	sta $207, y
	sta $20b, y
	sta $20f, y
	adc #$08
	jmp do_move
move_left:
	lda $700, x
	sec
	sbc #$01
	sta $700, x
	sta $203, y		; move all 16 sprites left or right
	sta $207, y
	sta $20b, y
	sta $20f, y
	adc #$07
do_move:
	sta $213, y
	sta $217, y
	sta $21b, y
	sta $21f, y
	adc #$08
	sta $223, y
	sta $227, y
	sta $22b, y
	sta $22f, y
	adc #$08
	sta $233, y
	sta $237, y
	sta $23b, y
	sta $23f, y
	tya
	clc
	adc #$40
	beq move_done
	tay
	txa
	clc
	adc #$10
	tax
	jmp start_move	
move_done:
	rts
	
init_sprites:
	lda #$10	;x-pos
	sta $700
	sta $701	;y-pos
	lda #$01
	sta $702	;direction
	lda #$00
	sta $703 	;top left tilenum
	lda #$02
	sta $704
	
	lda #$40
	sta $710
	sta $711
	lda #$00
	sta $712
	lda #$05
	sta $713
	lda #$00
	sta $714

	lda #$70
	sta $720
	sta $721
	lda #$01
	sta $722
	lda #$00
	sta $723
	lda #$02
	sta $724
	
	lda #$a0
	sta $730
	sta $731
	lda #$00
	sta $732
	lda #$05
	sta $733
	lda #$00
	sta $734
	
	; Write into sprite DMA area
	ldx #$00
	ldy #$00

set_tiles:
	lda $703, y
	clc
	sta $201, x
	adc #$01
	sta $211, x
	adc #$01
	sta $221, x
	adc #$01
	sta $231, x
	clc
	adc #$0d
	sta $205, x
	adc #$01
	sta $215, x
	adc #$01
	sta $225, x
	adc #$01
	sta $235, x
	clc
	adc #$0d
	sta $209, x
	adc #$01
	sta $219, x
	adc #$01
	sta $229, x
	adc #$01
	sta $239, x
	clc
	adc #$0d
	sta $20d, x
	adc #$01
	sta $21d, x
	adc #$01
	sta $22d, x
	adc #$01
	sta $23d, x
	txa
	clc
	adc #$40
	beq tiles_done
	tax
	tya
	clc
	adc #$10
	tay
	jmp set_tiles
tiles_done:

	; Write into sprite DMA area
	ldx #$00
	ldy #$00

set_y:
	lda $701, y
	sta $200, x
	sta $210, x
	sta $220, x
	sta $230, x
	clc
	adc #$08
	sta $204, x
	sta $214, x
	sta $224, x
	sta $234, x
	clc
	adc #$08
	sta $208, x
	sta $218, x
	sta $228, x
	sta $238, x
	clc
	adc #$08
	sta $20c, x
	sta $21c, x
	sta $22c, x
	sta $23c, x
	txa
	clc
	adc #$40
	beq y_done
	tax
	tya
	clc
	adc #$10
	tay
	jmp set_y
y_done:
	
	ldx #$00
	ldy #$00

set_pal:
	lda $704, y
	sta $202, x
	sta $212, x
	sta $222, x
	sta $232, x
	sta $206, x
	sta $216, x
	sta $226, x
	sta $236, x
	sta $20a, x
	sta $21a, x
	sta $22a, x
	sta $23a, x
	sta $20e, x
	sta $21e, x
	sta $22e, x
	sta $23e, x
	txa
	clc
	adc #$40
	beq pal_done
	tax
	tya
	clc
	adc #$10
	tay
	jmp set_pal
pal_done:
	rts

	
sprite_dma:
	lda #$02
	sta $4014
	rts
	
no_gfx:
	rts
	
gfx_func:				; Jumptable for PPU update routines
	.dw no_gfx, sprite_dma
		
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
	lda #$01			; Then we set them up so that the games regular NMI
	sta $ea				; code doesn't try to run any sound code at all and instead
	sta $e4				; will exit as fast as possible
	
	lda #$00			; Set a few more values to zero, including the RNG value.
	sta $45				; RNG is one of the few things that still update when the NMI is
	sta $46				; in lag-mode, so setting it to 0 means NMI will change it from 0 when it's done.

-	lda $46				; Wait here until the RNG value has changed from zero
	beq -				; meaning that NMI has just run

	jsr update_gfx		; Quickly do our own graphics update code while we're still in vblank
	rts	
	
paldata:
	.incbin "screensaver.pal"

.org $800			; Org here doesn't really do anything except pad the above payload to exactly $500 bytes	
ntdata:
	.incbin "screensaver.nam"
chrdata:
	.incbin "screensaver.chr"
	
