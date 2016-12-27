--Write to I/O register. Trashes A.
write_io = function(stream, addr, val)
	stream:lda_imm(val);
	stream:sta_mem(addr);
end

poll_sequence_core = function(stream)
	for i = 1,4 do
		stream:lda_dp(0);
		stream:asl_a();
		stream:asl_a();
		stream:eor_dp(0);
		stream:asl_a();
		stream:asl_a();
		stream:eor_dp(0);
		stream:asl_a();
		stream:asl_a();
		stream:eor_dp(0);
		stream:pha();
	end
end


--Loadbase points to loadbase of S5M!
--Note: This needs speed of at least 143 for S5M.
stage5p = function(stream, loadbase)
	-- Wait until next vblank starts
	stream:sep(0x20);
	stream:label("vf_not_in_vblank"); 
	stream:lda_mem(0x4212);
	stream:bpl("vf_not_in_vblank");
	--Init PPU.
	--write_io uses a register, so 0x20 affects it properly.
	--FIXME: This should clear the background of last restort color.
	write_io(stream, 0x420c, 0x00);		-- Turn those darn HDMAs off!
	write_io(stream, 0x2100, 0x8f);		-- Display off.
	write_io(stream, 0x2105, 0x03);		-- Tiles 8x8, mode 3.
	write_io(stream, 0x2107, 0x00);		-- BG1 tile data at 0k, 32x32 map.
	write_io(stream, 0x210B, 0x01);		-- BG1 tiles at 8k.
	write_io(stream, 0x210D, 0x00);		-- BG1 scroll (0,-1).
	write_io(stream, 0x210D, 0x00);
	write_io(stream, 0x210E, 0xFF);
	write_io(stream, 0x210E, 0x00);
	write_io(stream, 0x2115, 0x80);		-- VRAM increment 2 bytes on high access.
	write_io(stream, 0x212C, 0x01);		-- Enable only BG1.
	write_io(stream, 0x212D, 0x00);		-- Disable windows.
	write_io(stream, 0x212E, 0x00);		-- Disable windows.
	write_io(stream, 0x2130, 0x00);		-- Black background-of-last-resort.
	write_io(stream, 0x2131, 0x00);		-- Black background-of-last-resort.
	write_io(stream, 0x2132, 0xE0);		-- Black background-of-last-resort.
	--Clear CGRAM&VRAM.
	write_io(stream, 0x0000, 0x00);		-- Some zeroes
	write_io(stream, 0x0001, 0x00);		-- Some zeroes
	write_io(stream, 0x4300, 0x09);		-- Fixed address, A->B, 2 bytes.
	write_io(stream, 0x4301, 0x18);		-- DMA to $2118
	write_io(stream, 0x4302, 0x00);		-- DMA from $00:0000
	write_io(stream, 0x4303, 0x00);
	write_io(stream, 0x4304, 0x00);
	write_io(stream, 0x4305, 0x00);		-- DMA 64kB.
	write_io(stream, 0x4306, 0x00);
	write_io(stream, 0x2116, 0x00);		-- VRAM address 0.
	write_io(stream, 0x2117, 0x00);
	write_io(stream, 0x4310, 0x08);		-- Fixed address, A->B, 1 bytes.
	write_io(stream, 0x4311, 0x22);		-- DMA to $2122
	write_io(stream, 0x4312, 0x00);		-- DMA from $00:0000
	write_io(stream, 0x4313, 0x00);
	write_io(stream, 0x4314, 0x00);
	write_io(stream, 0x4315, 0x00);		-- DMA 512B.
	write_io(stream, 0x4316, 0x02);
	write_io(stream, 0x2121, 0x00);		-- CGRAM address 0.
	write_io(stream, 0x420b, 0x03);		-- Go for dma, 0&1.
	write_io(stream, 0x2100, 0x0f);		-- Display on.
	stream:rep(0x30);
	--Set stack to something sane.
	stream:lda_imm(0x1FF);
	stream:tcs();
	--Copy the code into SRAM...
	stream:per("start_of_code");
	stream:plx();
	stream:stx_dp(0xFC);
	stream:per("end_of_code");
	stream:pla();
	stream:clc();		--Substract one extra.
	stream:sbc_dp(0xFC);
	stream:ldy_imm(0x8000);
	stream:phb();
	stream:mvn(0xF0, 0x00);
	stream:plb();
	--Force FastROM.
	stream:lda_imm(0x0100);
	stream:sta_mem(0x420c);
	--Enter code in SRAM. This effectively jumps to start_of_code.
	stream:jml(0xF0, 0x8000);
	--This is the code in SRAM.
	stream:label("start_of_code");
	--First, we need to resynchronize, as we reach here in who knows what framephase.
	stream:sep(0x20);
	stream:label("vf_not_in_vblank2");
	stream:lda_mem(0x4212);
	stream:bpl("vf_not_in_vblank2");
	stream:brl("wait_frame_and_loop");	-- Complete sync by waiting for lowering edge.
	--This is the frame loop. We assume we are fresh out of vblank here, with 16-bit A and X.
	stream:label("frame_loop");
	stream:__assumeflags(0);	-- Due to rearranging of code, we do have flags 0x20 here.
	local speed = 150;			-- We use 261x speed.
	--Load DPBase=4016, stack on right address and latch controllers.
	stream:lda_imm(0x4016);
	stream:tcd();
	--stream:lda_imm(0x1000 + 8 * (2 * speed + 1) - 1);		-- WTF?
	stream:lda_imm(0x1000 + 8 * (2 * speed) - 1);			-- WTF?
	stream:tcs();
	stream:ldx_imm(speed);
	stream:lda_imm(1);
	stream:sta_dp(0);
	stream:stz_dp(0);
	--poll_sequence_core(stream);
	stream:label("poll_once2");
	poll_sequence_core(stream);
	poll_sequence_core(stream);
	stream:dex();
	stream:bne("poll_once2");
	--Restore DPBase. No need to restore stack, there's space below buffer.
	stream:lda_imm(0x0000);
	stream:tcd();

	-- Wait until next vblank starts, so we can manipulate video.
	stream:sep(0x20);
	stream:label("vf_not_in_vblank3");
	stream:lda_mem(0x4212);
	stream:bpl("vf_not_in_vblank3");

	--Do the actual video manipulation.
	stream:lda_mem(0x1000)		-- The first byte in loadbuffer.
	stream:cmp_imm(0x01)
	stream:beq("dma_vram_cmd")
	stream:cmp_imm(0x02)
	stream:beq("dma_cgram_cmd")
	stream:brl("wait_frame_and_loop")
	stream:label("dma_vram_cmd")
	stream:rep(0x30);
	stream:lda_mem(0x1001);			--WRAM write addres, load from offset 1 in first frame.
	stream:sta_mem(0x2116);
	stream:lda_imm(0x1801);			--2 bytes, to $2118, incrementing.
	stream:sta_mem(0x4300);
	stream:lda_imm(0x1008);			--Transfer from 8 bytes into buffer.
	stream:sta_mem(0x4302);
	stream:lda_imm(0x0000);			--Bank 0.
	stream:sta_mem(0x4304);			-- Get DMA size in words from offset 3 in first frame.
	stream:lda_mem(0x1003);			
	stream:sta_mem(0x4305);
	stream:lda_imm(0x0001);			
	stream:sta_mem(0x420b);			--Start DMA to VRAM.
	stream:brl("twiddle_bg1_regs")
	stream:label("dma_cgram_cmd")
	stream:__assumeflags(0x20);		-- Due to jump, we have flags 0x20 here.
	stream:lda_imm(0x00);			--CGRAM write addres
	stream:sta_mem(0x2121);
	stream:rep(0x30);
	stream:lda_imm(0x2200);			--1 byte, to $2122, incrementing.
	stream:sta_mem(0x4300);
	stream:lda_imm(0x1008);			--Transfer from 8 bytes into buffer.
	stream:sta_mem(0x4302);
	stream:lda_imm(0x0000);			--Bank 0, transfer xx00 bytes.
	stream:sta_mem(0x4304);
	stream:lda_imm(0x0002);			--Transfer 02xx bytes.
	stream:sta_mem(0x4306);
	stream:lda_imm(0x0001);
	stream:sta_mem(0x420b);			--Start DMA to CGRAM.
	--Fallthrough.
	stream:label("twiddle_bg1_regs")
	stream:sep(0x20);
	stream:lda_mem(0x1005);			-- Load BG1 tile mode from offset 5 in first frame.
	stream:sta_mem(0x2107);
	stream:lda_mem(0x1006);			-- Load BG1 tile offset from offset 6 in first frame.
	stream:sta_mem(0x210B);
	stream:brl("wait_frame_and_loop")

	-- Wait until this vblank ends.
	stream:label("wait_frame_and_loop");
	stream:sep(0x20);
	stream:label("vf_in_vblank");
	stream:lda_mem(0x4212);
	stream:bmi("vf_in_vblank");
	stream:rep(0x20);
	stream:brl("frame_loop");

	stream:label("end_of_code");
	return stream:__dumpcode();
end
