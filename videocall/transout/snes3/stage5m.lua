--The frame loop offset.
S5M_FRAMELOOP_OFF = 0x1D;
--The offset of frame store end address - 1 in the code.
S5M_END_ADDR_OFF = 0x22;
--The offset of frame store count in the code.
S5M_FRAMECOUNT_OFF = 0x26;
--The length of code
S5M_CODE_LENGTH = 114;


--Emit poll sequence. Trashes A, assumes DPbase=0x4016 and 16-bit A.
poll_sequence = function(stream)
	stream:lda_imm(1);
	stream:sta_dp(0);
	stream:stz_dp(0);
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

--Emit entiere multi-poll sequence, writing in reverse from addr Trashes A, X, SP and DPBase. Assumes 16-bit A/X.
poll_sequence_multiple = function(stream, addr, count)
	stream:lda_imm(0x4016);
	stream:tcd();
	stream:lda_imm(addr + 8 * count - 1);		-- WTF?
	stream:tcs();
	stream:ldx_imm(count);
	stream:label("poll_once");
	poll_sequence(stream);
	stream:dex();
	stream:bne("poll_once");
	stream:lda_imm(0x0000);
	stream:tcd();
end

stage5m = function(stream, loadbase, speed)
	--Init CPU
	stream:rep(0x38);
	stream:lda_imm(0x0000);
	stream:tcd();
	stream:lda_imm(0x1fff);
	stream:tcs();
	--Wait for next frame.
	stream:sep(0x20);
	stream:lda_imm(0x00);					-- All interrupts off (including autopoll).
	stream:sta_mem(0x4200);
	stream:label("vf_not_in_vblank");                       -- Wait until next vblank starts
	stream:lda_mem(0x4212);
	stream:bpl("vf_not_in_vblank");
	stream:label("vf_in_vblank");                           -- Wait until this vblank ends.
	stream:lda_mem(0x4212);
	stream:bmi("vf_in_vblank");
	stream:rep(0x20);
	--Assume we are at start of frame. This code is 114 bytes big, that's what that 114 comes from.
	poll_sequence_multiple(stream, loadbase+S5M_CODE_LENGTH, speed);
	--The rest is intentionally missing. The poll above loads whatever code here, so execution will fall
	--through.
	return stream:__dumpcode();
end
