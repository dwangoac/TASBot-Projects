stage4 = function(stream, baseaddr, entry, size)
	stream:__assumeflags(0x30);
	stream:sep(0x30);		-- 8 bit A/X/Y.
	stream:lda_imm(1)		-- Autopoll on, NMI and IRQ off.
	stream:sta_mem(0x4200);
	stream:lda_imm(0)		-- Turn those damn HDMAs off!
	stream:sta_mem(0x420c);
	stream:rep(0x10);		-- 16 bit X/Y.
	stream:ldy_imm(baseaddr);	-- Write address  
	stream:label("loop");
	stream:label("not_in_vblank");	-- Wait until next vblank ends
	stream:lda_mem(0x4212);
	stream:bpl("not_in_vblank");
	stream:label("in_vblank");
	stream:lda_mem(0x4212);
	stream:bmi("in_vblank");
	stream:ldx_imm(0x4218);		-- Copy from 4218
	stream:lda_imm(0);		-- Copy 7 bytes (we have to init high byte too).
	stream:xba();
	stream:lda_imm(7);
	stream:phb();			-- Save bank (MVN trashes it)
	stream:mvn(0x7E, 0x00);
	stream:plb();
	stream:cpy_imm(baseaddr + size);
	stream:bne("loop");
	stream:jml(0x7E, entry);
	stream:nop();
	return stream:__dumpcode();
end
--[[
dofile("macroasm.lua");
local out = io.open("stage4-exp.cmp", "w");
out:write("0x001900\n");
local base = 0x1900;
local s4code = stage4(snes_libasm(base), 0x1a00, 0x1a08, 0x200);
for i=1,#s4code do
	out:write(string.format("0x%04x 0x%02x\n", base + i - 1, s4code[i]));
end
out:close();
print("ok");
]]
