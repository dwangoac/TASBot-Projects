.include "rommap.i"
.org $01B0
						; Stage 2 loader, this is written by the stage 1 loader using PHA
						; onto the top of the stack, the code in this has the limitation of
						; being input by "newly pressed buttons" only and thus cannot have the
						; same bit set in any consecutive byte. This is solved by using some
						; unofficial opcodes like IGN and SKP as padding. We can also not use
						; $00 because that's the marker for ending stage 1 transfer.
	
	.db $00				; End of stage 2 marker - used by stage 1 loader
	.db $b8, $01		; Address stage 1 loader will jump to
	.db $04, $01		; IGN $01 - We use these as a NOP Sled into stage 2 loader
	.db $04, $01		; IGN $01
	.db $04, $01		; IGN $01
	.db $04, $01		; IGN $01
	.db $04, $01		; IGN $01
	.db $04, $01		; IGN $01
	.db $04, $01		; IGN $01
	.db $04, $01		; IGN $01
	.db $04, $01		; IGN $01
	.db $04, $01		; IGN $01
	
	ldx #$01			; Set stack pointer to $01 so we can overwrite the start of this code with the new loader
	.db $04, $01		; with the stack messing it up.
	txs				
	.db $04, $01		; IGN $01
	
	ldx #$0d
	ldy #$01
	dey
-	jsr $c01b
	.db $04, $02		; IGN $02
	lda $08, x
	.db $04, $02		; IGN $02
	sta $0102, y		; Start writing at $102
	iny
	.db $04, $02		; IGN $02
	lda $18				; Check for controller 1 new input and exit if $80 or higher is pressed
	.db $04, $02		; IGN $02
	bpl -
	bpl +
	.db $04, $02		; IGN $02
+	jmp $0102
	.db $04, $01		; This is just padding so that we offset the code enough so that the stack won't overwrite it
	.db $04, $01		; after we adjust the stack to $01 above.
