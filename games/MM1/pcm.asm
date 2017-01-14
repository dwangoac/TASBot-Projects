.include "rommap.i"

; Chunk header, includes 16 bytes of sync-data and then the real header
.org $2de
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $00
	
	.db $01, $80, $00, $03		; Copy with PPU on, set $2000 to $80 while copying, and then jump to $0300
	.db $00, $05, $00, $03		; Copy 2 banks of RAM data to $0300 (this code)
	.db $01, $04, $00, $24		; Copy 4 banks of PPU data to $2000 (nametable + attributes)
	.db $01, $10, $00, $10		; Copy 16 banks of PPU data to $0000 (one bank of CHR data)
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
	

pcm_vbv1:    
	bit $2002  ; wait for vblank
	bpl pcm_vbv1

	ldx #$00
	lda #$3f
	sta $2006
	lda #$00
	sta $2006

pcm_writepal:
	lda.w pcm_paldata,x
	sta $2007
	inx
	cpx #$10
	bne pcm_writepal

	lda #%00001110
	sta $2001      


pcm_vbv3:
	bit $2002  ; wait for vblank
	bpl pcm_vbv3

	lda #$00
	sta $2005
	sta $2005
	sta $2006
	sta $2006
	lda #$11
	sta $2000
	
	lda #$00
	ldx #$01
	ldy #$00
	
	stx $4016       ;4
	sty $4016       ;4
	stx $00
	stx $01
	
	stx $4016       ;4
	sty $4016       ;4

pcm_start:

	LDA $4017       ;4
	ASL A           ;2

	EOR $4017
	ASL A

	EOR $4017
	ASL A

	EOR $4017
	STA $4011

	ASL A           ;2
	STA $00         ;3
	STA $00         ;3

	LDA $4017       ;4
	ASL A           ;2

	EOR $4017
	ASL A

	EOR $4017
	ASL A

	EOR $4017
	STA $4011

	LDA $4017       ;4
	ASL A           ;2

	EOR $4017
	ASL A

	EOR $4017
	ASL A

	EOR $4017
	STA $4011

	LDA #%10100000  ;2
	AND $00         ;3
	STA $00         ;3

	LDA $4017       ;4
	ASL A           ;2

	EOR $4017
	ASL A

	EOR $4017
	ASL A

	EOR $4017
	STA $4011

	stx $4016       ;4
	sty $4016       ;4
	
	LDA $4016       ;4
	ASL A           ;2

	EOR $4016
	ASL A

	EOR $4016
	ASL A

	EOR $4016
	STA $4011

	LDA #%00001110  ;4
	ORA $00         ;4
	STA $00         ;4

	LDA $4016       ;4
	ASL A           ;2

	EOR $4016
	ASL A

	EOR $4016
	ASL A

	EOR $4016
	STA $4011

	LDA $0800       ; 4
	STA $2001       ; 4

	LDA $4016       ;4
	ASL A           ;2

	EOR $4016
	ASL A

	EOR $4016
	ASL A

	EOR $4016
	STA $4011

	LDA $7FFF       ;4
	STA $7FFF       ;4

	LDA $4016       ;4
	ASL A           ;2

	EOR $4016       ;4
	ASL A           ;2

	EOR $4016       ;4
	ASL A           ;2

	EOR $4016       ;4
	STA $4011       ;4

	ROL $00         ;5
	JMP pcm_start       ;3
	
pcm_paldata:
	.incbin "pcm.pal"
	
.org $800			; Org here doesn't really do anything except pad the above payload to exactly $500 bytes	
pcm_ntdata:
	.incbin "pcm.nam"
pcm_chrdata:
	.incbin "pcm.chr"
	
