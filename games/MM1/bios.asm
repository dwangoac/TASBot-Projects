.include "rommap.i"

; Chunk header, includes 16 bytes of sync-data and then the real header
.org $2de
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $00
	
	.db $01, $80, $00, $03		; Copy with PPU on, set $2000 to $90 while copying, and then jump to $0300
	.db $00, $05, $00, $03		; Copy 5 banks of RAM data to $0300 (this code)
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
	stx $2005
	stx $2005
	stx $2006
	stx $2006

	lda #$01
	sta $1f
	sta $ea
	sta $e4

	lda #$80
	sta $2000
	
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
	sta $80
	sta $81
	sta $45
	sta $46	
	
main:
	jsr update_timer
	jsr wait_vblank
	lda $81
	cmp #$02
	bne +
	jmp $102		; Load next part
+	jmp main
	
update_timer:
	inc $80
	bne +
	inc $81
+ 	rts
	
wait_vblank:
	lda #$00
	sta $45
	sta $46
-	lda $46
	beq -
	rts	

paldata:
	.incbin "bios.pal"
	
.org $800			; Org here doesn't really do anything except pad the above payload to exactly $500 bytes	
pcm_ntdata:
	.incbin "bios.nam"
pcm_chrdata:
	.incbin "bios.chr"
	
