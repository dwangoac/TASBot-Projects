.include "rommap.i"
.org $5F00

; We set origin to $5F00 and pad with $FF's here to be able to cleanly sync to
; the input stream from the replay device, as soon as we hit $00 after $FF we
; know that we are in sync and can start writing the final payload to $6000

    .db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
    .db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$00

; We're at $6000 - start of NES SRAM
payload: 
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
	stx $a000


vbv1:
    bit $2002  ; wait for vblank
    bpl vbv1

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


	ldx #$00
	lda #$20
	sta $2006
	lda #$00
	sta $2006

writent1:
	lda.w ntdata,x
	sta $2007
	inx
	bne writent1

	ldx #$00

writent2:
	lda.w ntdata+$100, x
	sta $2007
	inx
	bne writent2

	ldx #$00

writent3:
	lda.w ntdata+$200,x
	sta $2007
	inx
	bne writent3

	ldx #$00
	
writent4:
	lda.w ntdata+$300,x
	sta $2007
	inx
	bne writent4

	ldx #$00


	ldx #$00
	lda #$24
	sta $2006
	lda #$00
	sta $2006

writent1a:
	lda.w ntdata,x
	sta $2007
	inx
	bne writent1a

	ldx #$00

writent2a:
	lda.w ntdata+$100,x
	sta $2007
	inx
	bne writent2a

	ldx #$00
	
writent3a:
	lda.w ntdata+$200,x
	sta $2007
	inx
	bne writent3a

	ldx #$00
	
writent4a:
	lda.w ntdata+$300,x
	sta $2007
	inx
	bne writent4a


	ldx #$00
clearmusic:
	sta $400,x      ; Clear $400-$4ff here to stop SMB3's music code
	inx             ; from playing any music.
	bne clearmusic  

	lda #$00
	ldx #$00
clearsprites:
	sta $500,x      ; Clear $500-$5ff for Sprite DMA
	inx             
	bne clearsprites


	lda #72
	sta $500
	lda #$fe
	sta $501
	lda #$00
	sta $502
	lda #248
	sta $503



vbv2: 
	bit $2002
	bpl vbv2

	lda #$00        ; Sprite DMA
	sta $2003
	lda #$05
	sta $4014

	lda #$40
	sta $8000
	lda #$5c
	sta $8001
	lda #$41
	sta $8000
	lda #$5e
	sta $8001

	lda #%00011110
	sta $2001      

	lda #%10000000  ; turn NMI back on now that we've set memory up
	sta $2000       ; so that it does as little as possible and just
					; increases $15 so we can use that to sync with

	ldx #<musicdata
	ldy #>musicdata
	jsr famitone    ; init routine

	lda #$00
	jsr famitone+$93 ; start playing song 00
	lda #$00
	sta $200
	sta $201
	sta $202
	sta $230
	sta $240
	sta $241
	sta $242
	sta $243
	sta $244
	sta $245

	lda #$27
	sta $244
	lda #$1F
	sta $245
	lda #$07
	sta $240
	lda #$ff
	sta $243
	lda #$00
	sta $246
	lda #%10000000
	sta $247



loop:
	jsr famitone+$124       ; play update routine


	lda $15                 ; check for when to switch to PCM
	cmp #$ff
	bne cont_lp
	inc $200

cont_lp:
	lda $200
	cmp #$0C
	bne cont_lp2
	
	lda $15
	cmp #$EF
	bne cont_lp2

	lda #$00
	sta $4015
	sta $2000
	sta $2001
	jmp $7000

cont_lp2:
							; figure out palette swap
	lda $15
	bne nopal
	inc $201
	lda $201
	cmp #$04
	bne nextpal

	lda #$00
	sta $201
nextpal:
	inc $202

nopal:

	lda #$00
	sta $210
	sta $211
	inc $212
	inc $230

WaitHitClear:
	bit $2002
	bvs WaitHitClear
	
WaitHitSet:
	bit $2002
	bvc WaitHitSet
	
	jsr wavy
	jsr scrolltext


	jsr wait_vbl            ; Main loop done, wait for NMI to end
	jmp loop
	nop
	nop
	

scrolltext:
	inc $240
	lda $240
	cmp #$08
	bne scroll_cont
	lda #$00
	sta $240
	inc $243
	inc $245

	lda $245
	cmp #$40
	bne scroll_cont

	lda $244
	cmp #$27
	bne scroll_ch1
	lda #$23
	sta $244
	jmp scroll_ch2
scroll_ch1:
	lda #$27
	sta $244
scroll_ch2:
	lda #$20
	sta $245

scroll_cont:


	lda $230
	cmp #$ff
	bne scroll_cont2
	lda $246
	bne scroll_nt1
	lda #%10000001
	sta $247
	inc $246
	jmp scroll_cont2
scroll_nt1:
	lda #%10000000
	sta $247
	dec $246

scroll_cont2:
	ldx $243
	lda.w scrolldata,x
	bne scroll_cont3
	lda #$00
	sta $243

scroll_cont3:
	rts
	

wait_vbl:           ; This routine waits until $15 gets changed, meaning
	lda $15         ; that SMB3's NMI routine has run, and we exit the        
loop_vbl:           ; waiting loop at scanline 245 here, meaning we have
	cmp $15         ; 16 scanlines of VBL time left for ourselves.
	beq loop_vbl    ; SMB3's own NMI steals 4 scanlines worth of cycles
					; from us.


					; Do NMI things
	lda $2002

	lda #$00        ; Sprite DMA
	sta $2003
	lda #$05
	sta $4014

	lda #$00
	sta $2005
	sta $2005


					; Scrolltext
	lda $240        ; scroll timer (whenever it is 0, write new char)
	bne pal_change
	
	lda $244        ; ppu high
	sta $2006
	lda $245        ; ppu low
	sta $2006

	ldx $243        ; char index
	lda.w scrolldata,x
	sta $2007       ; write data


pal_change:
	lda $202
	beq cont_vbl
	dec $202

	lda $201
	cmp #$00
	bne p1
	jsr writepal1
	jmp cont_vbl
p1:
	cmp #$01
	bne p2
	jsr writepal2
	jmp cont_vbl
p2:
	cmp #$02
	bne p3
	jsr writepal3
	jmp cont_vbl
p3:
	cmp #$03
	bne p4
	jsr writepal4
p4:

cont_vbl:
	
	ldx #$00
	stx $2005
	stx $2005
	stx $2006
	stx $2006

	rts             ; Return back to main loop
	

writepal1:
	ldx #$00
	lda #$3f
	sta $2006
	lda #$00
	sta $2006
pal_loop1:
	lda.w paldata,x
	sta $2007
	inx
	cpx #$10
	bne pal_loop1
	rts

writepal2:
	ldx #$00
	lda #$3f
	sta $2006
	lda #$00
	sta $2006
pal_loop2:
	lda.w paldata2,x
	sta $2007
	inx
	cpx #$10
	bne pal_loop2
	rts

writepal3:
	ldx #$00
	lda #$3f
	sta $2006
	lda #$00
	sta $2006
pal_loop3:
	lda.w paldata3,x
	sta $2007
	inx
	cpx #$10
	bne pal_loop3
	rts

writepal4:
	ldx #$00
	lda #$3f
	sta $2006
	lda #$00
	sta $2006
pal_loop4:
	lda.w paldata4,x
	sta $2007
	inx
	cpx #$10
	bne pal_loop4
	rts


wavy:
	lda $0210       ;4
	sta $2005       ;4
	inc $0211       ;6

	lda $0211       ;4 current line
	clc             ;2
	adc $0212       ;4 sine wave offset
	tax             ;2
	lda.w sinewave,x  ;4 load sine value
	lsr             ;2 shift right to make value smaller
	lsr             ;2
	sta $0210        ;4 store for next row
	
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop             ; 20 cycles (10 nops)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop             ; 20 cycles (10 nops)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop             ; 20 cycles (10 nops)

	nop
	nop
	nop
	nop

	lda $0211       ;4
	cmp #$30        ;2
	bne wavy        ;3 = 47
	lda $230
	sta $2005
	lda $247
	sta $2000
	rts

paldata:
	.db $20,$38,$28,$0f,$20,$21,$11,$0f,$20,$26,$16,$0f,$20,$2a,$1a,$0f
paldata2:
	.db $20,$2a,$1a,$0f,$20,$38,$28,$0f,$20,$21,$11,$0f,$20,$26,$16,$0f
paldata3:
	.db $20,$26,$16,$0f,$20,$2a,$1a,$0f,$20,$38,$28,$0f,$20,$21,$11,$0f
paldata4:
	.db $20,$21,$11,$0f,$20,$26,$16,$0f,$20,$2a,$1a,$0f,$20,$38,$28,$0f

ntdata:
	.incbin "pwn.nam"

sinewave:
  .db $80, $83, $86, $89, $8C, $90, $93, $96
  .db $99, $9C, $9F, $A2, $A5, $A8, $AB, $AE
  .db $B1, $B3, $B6, $B9, $BC, $BF, $C1, $C4
  .db $C7, $C9, $CC, $CE, $D1, $D3, $D5, $D8
  .db $DA, $DC, $DE, $E0, $E2, $E4, $E6, $E8
  .db $EA, $EB, $ED, $EF, $F0, $F1, $F3, $F4
  .db $F5, $F6, $F8, $F9, $FA, $FA, $FB, $FC
  .db $FD, $FD, $FE, $FE, $FE, $FF, $FF, $FF
  .db $FF, $FF, $FF, $FF, $FE, $FE, $FE, $FD
  .db $FD, $FC, $FB, $FA, $FA, $F9, $F8, $F6
  .db $F5, $F4, $F3, $F1, $F0, $EF, $ED, $EB
  .db $EA, $E8, $E6, $E4, $E2, $E0, $DE, $DC
  .db $DA, $D8, $D5, $D3, $D1, $CE, $CC, $C9
  .db $C7, $C4, $C1, $BF, $BC, $B9, $B6, $B3
  .db $B1, $AE, $AB, $A8, $A5, $A2, $9F, $9C
  .db $99, $96, $93, $90, $8C, $89, $86, $83
  .db $80, $7D, $7A, $77, $74, $70, $6D, $6A
  .db $67, $64, $61, $5E, $5B, $58, $55, $52
  .db $4F, $4D, $4A, $47, $44, $41, $3F, $3C
  .db $39, $37, $34, $32, $2F, $2D, $2B, $28
  .db $26, $24, $22, $20, $1E, $1C, $1A, $18
  .db $16, $15, $13, $11, $10, $0F, $0D, $0C
  .db $0B, $0A, $08, $07, $06, $06, $05, $04
  .db $03, $03, $02, $02, $02, $01, $01, $01
  .db $01, $01, $01, $01, $02, $02, $02, $03
  .db $03, $04, $05, $06, $06, $07, $08, $0A
  .db $0B, $0C, $0D, $0F, $10, $11, $13, $15
  .db $16, $18, $1A, $1C, $1E, $20, $22, $24
  .db $26, $28, $2B, $2D, $2F, $32, $34, $37
  .db $39, $3C, $3F, $41, $44, $47, $4A, $4D
  .db $4F, $52, $55, $58, $5B, $5E, $61, $64
  .db $67, $6A, $6D, $70, $74, $77, $7A, $7D

; Generated by scrolltext2hex.py
; "Hello AGDQ 2017! This quick demo was programmed by total, has music by fuzyll, and was made possibe with help from Masterjun, Ilari, LordTom, micro500, Ownasaurus, kodewerx, p4plus2, serisium, and dwangoAC. Enjoy this water level remix by fuzyll.  "
scrolldata:
    .db $b7,$d4,$db,$db,$de,$fe,$b0,$b6,$b3,$c0,$fe,$f2,$f0,$f1,$f7,$ea,$fe,$c3,$d7,$d8,$cc,$fe,$ca,$ce,$d8,$d2,$da,$fe,$d3,$d4,$dc,$de,$fe,$81,$d0,$cc,$fe,$df,$cb,$de,$d6,$cb,$d0,$dc,$dc,$d4,$d3,$fe,$d1,$8c,$fe,$cd,$de,$cd,$d0,$db,$9a,$fe,$d7,$d0,$cc,$fe,$dc,$ce,$cc,$d8,$d2,$fe,$d1,$8c,$fe,$d5,$ce,$8f,$8c,$db,$db,$9a,$fe,$d0,$dd,$d3,$fe,$81,$d0,$cc,$fe,$dc,$d0,$d3,$d4,$fe,$df,$de,$cc,$cc,$d8,$d1,$db,$d4,$fe,$81,$d8,$cd,$d7,$fe,$d7,$d4,$db,$df,$fe,$d5,$cb,$de,$dc,$fe,$bc,$d0,$cc,$cd,$d4,$cb,$d9,$ce,$dd,$9a,$fe,$b8,$db,$d0,$cb,$d8,$9a,$fe,$bb,$de,$cb,$d3,$c3,$de,$dc,$9a,$fe,$dc,$d8,$d2,$cb,$de,$f5,$f0,$f0,$9a,$fe,$be,$81,$dd,$d0,$cc,$d0,$ce,$cb,$ce,$cc,$9a,$fe,$da,$de,$d3,$d4,$81,$d4,$cb,$88,$9a,$fe,$df,$f4,$df,$db,$ce,$cc,$f2,$9a,$fe,$cc,$d4,$cb,$d8,$cc,$d8,$ce,$dc,$9a,$fe,$d0,$dd,$d3,$fe,$d3,$81,$d0,$dd,$d6,$de,$b0,$b2,$e9,$fe,$b4,$dd,$d9,$de,$8c,$fe,$cd,$d7,$d8,$cc,$fe,$81,$d0,$cd,$d4,$cb,$fe,$db,$d4,$cf,$d4,$db,$fe,$cb,$d4,$dc,$d8,$88,$fe,$d1,$8c,$fe,$d5,$ce,$8f,$8c,$db,$db,$e9,$fe,$fe,$00

.org $7000
    .include "pcm.asm"

.org $7800
musicdata:
    .incbin "famitone2\music.bin"

.org $7BF0
famitone:
    .incbin "famitone2\player.bin"        
        
.org $7FFF
        .db $FF        ; Pad to exactly 8 kB of data
enddata:
         
