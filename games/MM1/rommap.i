; This allocates the full NES address space into a single bank so we can
; just use .org to specify the location of the code since we're not using
; ROM or any kind of bankswapping.

.ROMBANKMAP
BANKSTOTAL 1
BANKSIZE $10000
BANKS 1
.ENDRO

.MEMORYMAP
DEFAULTSLOT 0
SLOTSIZE $10000
SLOT 0 $0000
.ENDME