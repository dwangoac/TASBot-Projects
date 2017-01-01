.include "rommap.i"
.org $482
					  ; This file is just for documentation and should not be compiled since it's done by inputs already
					  
loop:                 ; Stage 1 loader (written by beam and magnet beam X positions)
	pha				  ; Push to stack
	jsr $c01b		  ; This calls controller reading AND "wait-for-NMI" so this limits it to 1 byte per frame
	bne loop		  ; The previous routine will leave "newly pressed buttons on controller 2" in A
	rts				  ; Returns to the last 2 bytes read from controller input that was pushed to the stack