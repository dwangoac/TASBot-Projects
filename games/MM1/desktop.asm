.include "rommap.i"

; Chunk header, includes 16 bytes of sync-data and then the real header
.org $2de
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $00
	
	.db $01, $91, $00, $03		; Copy with PPU on, set $2000 to $80 while copying, and then jump to $0300
	.db $00, $05, $00, $03		; Copy 2 banks of RAM data to $0300 (this code)
	.db $01, $04, $00, $20		; Copy 4 banks of PPU data to $2000 (nametable + attributes)
	.db $01, $10, $00, $00		; Copy 16 banks of PPU data to $0000 (one bank of CHR data)
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
	
	lda #$00
	sta $2005
	sta $2005
	sta $2006
	sta $2006
	lda #$80
	sta $2000
	sta $ff
	
	jsr wait_vblank

	ldx #$00
	lda #$3f
	sta $2006
	lda #$00
	sta $2006

writepal:
	lda.w paldata,x
	sta $2007
	inx
	cpx #$10
	bne writepal

	lda #%00001110
	sta $2001      

	jsr wait_vblank
	
	lda #$00
	sta $2005
	sta $2005
	sta $2006
	sta $2006
	lda #$80
	sta $2000
	
main:
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	lda #60
	jsr wait_frames
	
	jmp $0102	; load next part	
	
no_gfx:
	rts
	
gfx_func:				; Jumptable for PPU update routines
	.dw no_gfx
		
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
	.incbin "desktop.pal"
	
.org $800			; Org here doesn't really do anything except pad the above payload to exactly $500 bytes	
ntdata:
	.incbin "desktop.nam"
chrdata:
	.incbin "desktop.chr"
	
