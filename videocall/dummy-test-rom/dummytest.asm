;==LoRom==
.MEMORYMAP
  SLOTSIZE $8000
  DEFAULTSLOT 0
  SLOT 0 $8000
.ENDME
 
.ROMBANKSIZE $8000
.ROMBANKS 1
 
.SNESHEADER
  ID "SNES"
  NAME "LoZ Payload Test     "
  ;    "123456789012345678901"
   SLOWROM
  LOROM
  CARTRIDGETYPE $02		; ROM+SRAM
  ROMSIZE $05			; 32kB.
  SRAMSIZE $03                  ; 8kB
  COUNTRY $01                   ; USA
  LICENSEECODE $01
  VERSION $00
.ENDSNES
 
.SNESNATIVEVECTOR               ; Define Native Mode interrupt vector table
  COP EmptyHandler
  BRK EmptyHandler
  ABORT EmptyHandler
  NMI VBlank
  IRQ EmptyHandler
.ENDNATIVEVECTOR
 
.SNESEMUVECTOR                  ; Define Emulation Mode interrupt vector table
  COP EmptyHandler
  ABORT EmptyHandler
  NMI EmptyHandler
  RESET Start
  IRQBRK EmptyHandler
.ENDEMUVECTOR


.BANK 0 SLOT 0
.ORG 0
.SECTION "Dummy" SEMIFREE
EmptyHandler:
	RTI
.ENDS
 
Start:
	; Enter native mode.
	CLC
	XCE
	; 16-bit mode, clear decimal flag.
	REP #$38
	; Medium timings
	LDA #$0000
	STA $420c
	; Load stack.
	LDA #$1fff
	TCS
	; Set DB=0.
	LDA #$0000
	PHA
	PLB
	PLB
	; Load DP=0
	LDA #$0000
	TCD
	; Autopoller on, NMI on.
	LDA #$0081
	STA $4200
	; ...
	STZ $12
	; Main loop. If controller1 is B, we wait a frame and then jump to controller registers. otherwise
	; we loop until $12 becomes nonzero (NMI ran) and then zero it.
main_loop:
	; When $4218 is 1, jump to $4218 on the next frame.
	LDA $4218
	CMP #$8000
	BNE no_jump
	SEP #$20
wait1:
	LDA $4212
	BMI wait1
wait2:
	LDA $4212
	BPL wait1
	JMP $421A	; Jump to controller registers.
	REP #$20
no_jump:
	LDA $12
	BEQ no_jump
	STZ $12
	JMP main_loop


VBlank:
	; Just blow some time if $12 is zero. And set $12
	REP #$38	; Flags are autosaved by interrupt.
	PHA
	LDA $12
	BNE vblank_end_now
	LDA #$0100
wait_loop:
	DEC A
	BNE wait_loop
vblank_end_now:
	LDA #$01
	STA $12
	PLA
	RTI
	
 

.EMPTYFILL $00
