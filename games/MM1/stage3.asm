.include "rommap.i"

;NESChunkLoader
;
;$200-2FF is used for the loaders work-ram
;

; This header gets copied to $A0+
; If entry point is $0000, then we'll RTS instead of JMP since the loader was then called as a subroutine

;0x01, 0xPP, 0xBC, 0xAB	; PPU on/off, 	$2000 value, 	[entry point]		
;0x00, 0xSS, 0xBC, 0xAB	; RAM/PPU, 		banks, 			[target address]		
;0x00, 0xSS, 0xBC, 0xAB	; RAM/PPU, 		banks, 			[target address]
;0x00, 0xSS, 0xBC, 0xAB	; RAM/PPU, 		banks, 			[target address]
;...
;0xFF					; end of header

.org $102
sync:
    jsr read_joy    ; Call the games controller reading routine and read
					; data until we are not reading 0xFF, this is to sync
	cmp #$ff        ; the input stream so we know exactly what data we
	beq sync        ; are reading below.

	ldx #$00		; Read header into $a0+
-	jsr read_joy		
	sta $a0, x
	inx
	cmp #$ff
	bne -
	inc $1f
	inc $e4
	inc $ea
	ldx #$00
	lda $a0
	bne read_block
	stx $2001		; Disable PPU
	stx $2000		; Disable NMI

read_block:
	inx
	inx
	inx
	inx
	
	lda $a2, x
	sta $90
	lda $a3, x
	sta $91
	lda #$00
	sta $92
	lda $a0, x
	cmp #$ff			; Are we done?
	bne +
	jmp copy_done
+	cmp #$00
	beq copy_ram

copy_ppu:
	ldy #$00			; PPU copy
-	jsr read_joy		; First buffer 256 bytes into 0x300-0x3FF
	sta $200, y
	iny
	bne -
	
	jsr ppu_copy
	inc $92
	lda $92
	cmp $a1, x
	bne copy_ppu
	jmp read_block

copy_ram:
	ldy #$00
-	jsr read_joy
	sta ($90), y
	iny
	bne -	
	inc $91
	inc $92
	lda $92
	cmp $a1, x
	bne copy_ram
	jmp read_block
	
copy_done:
	lda $a3
	beq +
	jmp ($a2)
+	rts

ppu_copy:
	txa
	pha
	ldx #$00
--	ldy #$00
	lda $a0
	beq +
	jsr wait_vblank

+	lda $91
	sta $2006	
	stx $2006

-	lda $200, x
	sta $2007
	iny
	inx
	cpy #$40
	bne -
	lda $a0
	
	beq +
	
	lda $a1
	sta $2000
	lda #$00
	sta $2005
	sta $2005
	
+	cpx #$00
	bne --
	inc $91
	pla
	tax
	rts
	
wait_vblank:
	sty $45
	sty $46
	lda #$01
-	lda $46
	beq -
	rts
	
read_joy:
	txa
	pha
	tya
	pha
	jsr $d58d
	pla
	tay
	pla
	tax
	lda $14
	rts