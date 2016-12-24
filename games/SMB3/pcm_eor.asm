pcm_payload:
	sei        ; ignore IRQs
	cld        ; disable decimal mode
	ldx #$40
	stx $4017  ; disable APU frame IRQ
	ldx #$ff
	txs        ; Set up stack
	inx        ; now X = 0
	stx $2000  ; disable NMI
	stx $2001  ; disable rendering
	stx $4010  ; disable DMC IRQs
	stx $2005
	stx $2005
	stx $2006
	stx $2006
	stx $e000


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


	ldx #$00
	lda #$20
	sta $2006
	lda #$00
	sta $2006

pcm_writent1:
	lda.w pcm_ntdata,x
	sta $2007
	inx
	bne pcm_writent1

	ldx #$00
pcm_writent2:
	lda.w pcm_ntdata+$100,x
	sta $2007
	inx
	bne pcm_writent2

	ldx #$00
pcm_writent3:
	lda.w pcm_ntdata+$200,x
	sta $2007
	inx
	bne pcm_writent3

	ldx #$00
pcm_writent4:
	lda.w pcm_ntdata+$300,x
	sta $2007
	inx
	cpx #$C0
	bne pcm_writent4

	ldx #$00
pcm_writeattr:
	lda.w pcm_attrdata,x
	sta $2007
	inx
	cpx #$40
	bne pcm_writeattr

	ldx #$00
	lda #$00

pcm_vbv2:
	bit $2002
	bpl pcm_vbv2

	lda #$40
	sta $8000
	lda #$5c
	sta $8001
	lda #$41
	sta $8000
	lda #$5e
	sta $8001

	lda #%00001110
	sta $2001      

	lda #$00
	stx $2005
	stx $2005
	stx $2006
	stx $2006

pcm_vbv3:
	bit $2002  ; wait for vblank
	bpl pcm_vbv3

	lda #$00
	ldx #$01
	ldy #$00

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
	ASL A

	EOR $4017
	ASL A

	EOR $4017
	ASL A

	EOR $4017
	STA $4011

	LDA $4017
	STA $7FFF       ;4

	LDA $4017       ;4
	ASL A           ;2

	EOR $4017
	ASL A

	EOR $4017
	ASL A

	EOR $4017
	ASL A

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
	ASL A

	EOR $4016
	ASL A

	EOR $4016
	ASL A

	EOR $4016
	STA $4011

	LDA $4016       ;4
	STA $7FFF       ;4

	LDA $4016       ;4
	ASL A           ;2

	EOR $4016
	ASL A

	EOR $4016
	ASL A

	EOR $4016
	ASL A

	EOR $4016
	ASL A

	EOR $4016
	ASL A

	EOR $4016
	STA $4011

	rol $00
	jmp pcm_start

pcm_paldata:
	.incbin "pcm.pal"
pcm_ntdata:
	.incbin "pcm.nam"
pcm_attrdata:
	.incbin "pcm.atr"
