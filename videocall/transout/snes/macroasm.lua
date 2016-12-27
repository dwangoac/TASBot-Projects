snes_libasm = function(origin)
	local func_call = function(value, func) 
		return function(self, ...) self:__write8(value); func(self, ...); end
	end
	local func_noarg = function(value) 
		return func_call(value, function() end);
	end
	local func_arg8 = function(value) 
		return func_call(value, function(self, val) self:__write8(val); end);
	end
	local func_argr8 = function(value)
		return func_call(value, function(self, val) self:__writer8(val); end);
	end
	local func_arg16 = function(value) 
		return func_call(value, function(self, val) self:__write16(val); end);
	end
	local func_argA = function(value) 
		return func_call(value, function(self, val) self:__writeA(val); end);
	end
	local func_argX = function(value) 
		return func_call(value, function(self, val) self:__writeX(val); end);
	end
	local func_reljmp = function(value) 
		return func_call(value, function(self, val) self:__reflabel8(val); end);
	end
	local func_argL = function(value) 
		return func_call(value, function(self, bank, addr) self:__write16(addr); self:__write8(bank); end);
	end

	return {
		["__allow_pic"] = origin ~= nil,
		["__dumpcode"] = function(self)
			for k,v in pairs(self.__undefined) do error("Undefined label "..k); end
			local minaddr = 0xFFFFFFFFFFFF
			local maxaddr = 0
			for addr, val in pairs(self.__storage) do
				if addr < minaddr then minaddr = addr; end
				if addr > maxaddr then maxaddr = addr; end
			end
			local arr = {};
			for i=minaddr,maxaddr do
				local val = self.__storage[i] or 0;
				arr[i - minaddr + 1] = val;
			end
			return arr;
		end,
		["__origin"] = origin,
		["__offset"] = 0,
		["__storage"] = {},
		["__flags"] = 0x30,
		["__labels"] = {},
		["__undefined"] = {},
		["__realaddr"] = function(self, offset)
			if not self.__origin then
				return offset;
			end
			return self.__origin + offset;
		end,
		["__reflabel8"] = function(self, label)
			if type(label) == "number" then
				self:__writer8(label)
				return;
			end
			self.__undefined[label] = self.__undefined[label] or {};
			self.__undefined[label][self:__realaddr(self.__offset, true)] = 8;
			self:__write8(0);
			self:__resolve(label);
		end,
		["__reflabel16"] = function(self, label, fixup)
			if type(label) == "number" then
				self:__writer16(label)
				return;
			end
			self.__undefined[label] = self.__undefined[label] or {};
			self.__undefined[label][self:__realaddr(self.__offset, true)] = 16 + 256 * fixup;
			self:__write16(0);
			self:__resolve(label);
		end,
		["__resolve"] = function(self, label)
			local target = self.__labels[label];
			if type(target) ~= "number" then
				return;		--Not yet.
			end
			local addrs = self.__undefined[label];
			if addrs == nil then return; end
			for addr, size in pairs(addrs) do
				if size == 8 then
					local offset = target - (addr + 1);
					if offset < -128 or offset > 127 then
						print(string.format("target=%04x, source=%04x, offset=%d",
							target, addr + 1, offset));
						print(debug.traceback("8-bit offset out of range!"));
						error("8-bit offset out of range");
					end
					self:__rawbyte(addr, bit.all(offset + 0x100, 0xFF));
				elseif bit.all(size, 0xFF) == 16 then
					--Hack: size bits contain backward offset.
					local offset = target - (addr + 2) - bit.lrshift(size, 8);
					offset = bit.all(offset + 0x10000, 0xFFFF)
					self:__rawbyte(addr, bit.all(offset, 0xFF));
					self:__rawbyte(addr + 1, bit.lrshift(offset, 8));
				elseif size == 17 then
					if not self.__allow_pic then
						error("Absolute references not allowed in PIC.");
					end
					self:__rawbyte(addr, bit.all(target, 0xFF));
					self:__rawbyte(addr + 1, bit.lrshift(target, 8));
				else
					error("Internal error: Bad label size!");
				end
			end
			self.__undefined[label] = nil;
		end,
		["__assumeflags"] = function(self, flags)
			self.__flags = flags;
		end,
		["__setflags"] = function(self, flags)
			self.__flags = bit.any(self.__flags, flags);
		end,
		["__clearflags"] = function(self, flags)
			self.__flags = bit.all(self.__flags, bit.parity(flags, 0xFF));
		end,
		["__rawbyte"] = function(self, addr, val)
			self.__storage[addr] = val;
		end,
		["__write8"] = function(self, val)
			if val < 0 or val > 255 then 
				print(debug.traceback("8-bit value out of range!"));
				error("8-bit value out of range!");
			end
			self:__rawbyte(self:__realaddr(self.__offset, true), val);
			self.__offset = self.__offset + 1;
		end,
		["__writer8"] = function(self, val)
			if val < -128 or val > 127 then error("8-bit relative value out of range!"); end
			self:__rawbyte(self:__realaddr(self.__offset, true), bit.all(val + 0x100, 0xFF));
			self.__offset = self.__offset + 1;
		end,
		["__write16"] = function(self, val)
			if type(val) == "string" then 
				--Label.
				local label = val;
				self.__undefined[label] = self.__undefined[label] or {};
				self.__undefined[label][self:__realaddr(self.__offset, true)] = 17;
				self:__write16(0);
				self:__resolve(label);
				return
			end
			if val < 0 or val > 65535 then error("16-bit value out of range!"); end
			self:__rawbyte(self:__realaddr(self.__offset, true), bit.all(val, 0xFF));
			self:__rawbyte(self:__realaddr(self.__offset + 1, true), bit.lrshift(val, 8));
			self.__offset = self.__offset + 2;
		end,
		["__writer16"] = function(self, val)
			if val < -32768 or val > 32767 then error("16-bit relative value out of range!"); end
			self:__write16(bit.all(val + 0x10000, 0xFFFF));
		end,
		["__writeA"] = function(self, val)
			if bit.test_any(self.__flags, 0x20) then self:__write8(val); else self:__write16(val); end
		end,
		["__writeX"] = function(self, val)
			if bit.test_any(self.__flags, 0x10) then self:__write8(val); else self:__write16(val); end
		end,
		--ADC
		["adc_dp"] = func_arg8(0x65),
		["adc_dp_ind"] = func_arg8(0x72),
		["adc_dp_ind_x"] = func_arg8(0x61),
		["adc_dp_ind_y"] = func_arg8(0x71),
		["adc_dp_ind_long"] = func_arg8(0x67),
		["adc_dp_ind_long_y"] = func_arg8(0x77),
		["adc_dp_x"] = func_arg8(0x75),
		["adc_imm"] = func_argA(0x69),
		["adc_mem"] = func_arg16(0x6D),
		["adc_mem_x"] = func_arg16(0x7D),
		["adc_mem_y"] = func_arg16(0x79),
		["adc_long"] = func_argL(0x6F),
		["adc_long_x"] = func_argL(0x7F),
		["adc_sr"] = func_argr8(0x63),
		["adc_sr_ind_y"] = func_argr8(0x73),
		--AND
		["and_dp"] = func_arg8(0x25),
		["and_dp_ind"] = func_arg8(0x32),
		["and_dp_ind_x"] = func_arg8(0x21),
		["and_dp_ind_y"] = func_arg8(0x31),
		["and_dp_ind_long"] = func_arg8(0x27),
		["and_dp_ind_long_y"] = func_arg8(0x37),
		["and_dp_x"] = func_arg8(0x35),
		["and_imm"] = func_argA(0x29),
		["and_mem"] = func_arg16(0x2D),
		["and_mem_x"] = func_arg16(0x3D),
		["and_mem_y"] = func_arg16(0x39),
		["and_long"] = func_argL(0x2F),
		["and_long_x"] = func_argL(0x3F),
		["and_sr"] = func_argr8(0x23),
		["and_sr_ind_y"] = func_argr8(0x33),
		--ASL
		["asl_a"] = func_noarg(0x0A),
		["asl_dp"] = func_arg8(0x06),
		["asl_dp_x"] = func_arg16(0x16),
		["asl_mem"] = func_arg16(0x0E),
		["asl_mem_x"] = func_arg16(0x1E),
		--BCC
		["bcc"] = func_reljmp(0x90),
		--BCS
		["bcs"] = func_reljmp(0xB0),
		--BEQ
		["beq"] = func_reljmp(0xF0),
		--BIT
		["bit_dp"] = func_arg8(0x24),
		["bit_dp_x"] = func_arg8(0x34),
		["bit_imm"] = func_argA(0x89),
		["bit_mem"] = func_arg16(0x2C),
		["bit_mem_x"] = func_arg16(0x3C),
		--BMI
		["bmi"] = func_reljmp(0x30),
		--BNE
		["bne"] = func_reljmp(0xD0),
		--BPL
		["bpl"] = func_reljmp(0x10),
		--BRA
		["bra"] = func_reljmp(0x80),
		--BRK
		["brk"] = func_arg8(0x00),
		--BRL
		["brl"] = func_call(0x82, function(self, val) self:__reflabel16(val, 0); end),   --FIXME: Is this right?
		--BVC
		["bvc"] = func_reljmp(0x50),
		--BVS
		["bvs"] = func_reljmp(0x70),
		--CLC
		["clc"] = func_noarg(0x18),
		--CLD
		["cld"] = func_noarg(0xD8),
		--CLI
		["cli"] = func_noarg(0x58),
		--CLV
		["clv"] = func_noarg(0xB8),
		--CMP
		["cmp_dp"] = func_arg8(0xC5),
		["cmp_dp_ind"] = func_arg8(0xD2),
		["cmp_dp_ind_x"] = func_arg8(0xC1),
		["cmp_dp_ind_y"] = func_arg8(0xD1),
		["cmp_dp_ind_long"] = func_arg8(0xC7),
		["cmp_dp_ind_long_y"] = func_arg8(0xD7),
		["cmp_dp_x"] = func_arg8(0xD5),
		["cmp_imm"] = func_argA(0xC9),
		["cmp_mem"] = func_arg16(0xCD),
		["cmp_mem_x"] = func_arg16(0xDD),
		["cmp_mem_y"] = func_arg16(0xD9),
		["cmp_long"] = func_argL(0xCF),
		["cmp_long_x"] = func_argL(0xDF),
		["cmp_sr"] = func_argr8(0xC3),
		["cmp_sr_ind_y"] = func_argr8(0xD3),
		--COP
		["cop"] = func_arg8(0x02),
		--CPX
		["cpx_dp"] = func_arg8(0xE4),
		["cpx_imm"] = func_argX(0xE0),
		["cpx_mem"] = func_arg16(0xEC),
		--CPY
		["cpy_dp"] = func_arg8(0xC4),
		["cpy_imm"] = func_argX(0xC0),
		["cpy_mem"] = func_arg16(0xCC),
		--DEC
		["dec_a"] = func_noarg(0x3A),
		["dec_dp"] = func_arg8(0xC6),
		["dec_dp_x"] = func_arg16(0xD6),
		["dec_mem"] = func_arg16(0xCE),
		["dec_mem_x"] = func_arg16(0xDE),
		--DEX
		["dex"] = func_noarg(0xCA),
		--DEY
		["dey"] = func_noarg(0x88),
		--EOR
		["eor_dp"] = func_arg8(0x45),
		["eor_dp_ind"] = func_arg8(0x52),
		["eor_dp_ind_x"] = func_arg8(0x41),
		["eor_dp_ind_y"] = func_arg8(0x51),
		["eor_dp_ind_long"] = func_arg8(0x47),
		["eor_dp_ind_long_y"] = func_arg8(0x57),
		["eor_dp_x"] = func_arg8(0x55),
		["eor_imm"] = func_argA(0x49),
		["eor_mem"] = func_arg16(0x4D),
		["eor_mem_x"] = func_arg16(0x5D),
		["eor_mem_y"] = func_arg16(0x59),
		["eor_long"] = func_argL(0x4F),
		["eor_long_x"] = func_argL(0x5F),
		["eor_sr"] = func_argr8(0x43),
		["eor_sr_ind_y"] = func_argr8(0x53),
		--INC
		["inc_a"] = func_noarg(0x1A),
		["inc_dp"] = func_arg8(0xE6),
		["inc_dp_x"] = func_arg16(0xF6),
		["inc_mem"] = func_arg16(0xEE),
		["inc_mem_x"] = func_arg16(0xFE),
		--INX
		["inx"] = func_noarg(0xE8),
		--INY
		["iny"] = func_noarg(0xC8),
		--JMP
		["jmp"] = func_arg16(0x4C),
		["jmp_ind"] = func_arg16(0x6C),
		["jmp_ind_x"] = func_arg16(0x7C),
		--JML
		["jml"] = func_argL(0x5C),
		["jml_ind"] = func_arg16(0xDC),
		--JSR
		["jsr"] = func_arg16(0x20),
		["jsr_ind_x"] = func_arg16(0xFC),
		--JSL
		["jsl"] = func_argL(0x22),
		--(label)
		["label"] = function(self, label)
			self.__labels[label] = self:__realaddr(self.__offset, true);
			self:__resolve(label);
		end,
		--LDA
		["lda_dp"] = func_arg8(0xA5),
		["lda_dp_ind"] = func_arg8(0xB2),
		["lda_dp_ind_x"] = func_arg8(0xA1),
		["lda_dp_ind_y"] = func_arg8(0xB1),
		["lda_dp_ind_long"] = func_arg8(0xA7),
		["lda_dp_ind_long_y"] = func_arg8(0xB7),
		["lda_dp_x"] = func_arg8(0xB5),
		["lda_imm"] = func_argA(0xA9),
		["lda_mem"] = func_arg16(0xAD),
		["lda_mem_x"] = func_arg16(0xBD),
		["lda_mem_y"] = func_arg16(0xB9),
		["lda_long"] = func_argL(0xAF),
		["lda_long_x"] = func_argL(0xBF),
		["lda_sr"] = func_argr8(0xA3),
		["lda_sr_ind_y"] = func_argr8(0xB3),
		--LDX
		["ldx_dp"] = func_arg8(0xA6),
		["ldx_dp_y"] = func_arg8(0xB6),
		["ldx_imm"] = func_argX(0xA2),
		["ldx_mem"] = func_arg16(0xAE),
		["ldx_mem_y"] = func_arg16(0xBE),
		--LDY
		["ldy_dp"] = func_arg8(0xA4),
		["ldy_dp_x"] = func_arg8(0xB4),
		["ldy_imm"] = func_argX(0xA0),
		["ldy_mem"] = func_arg16(0xAC),
		["ldy_mem_x"] = func_arg16(0xBC),
		--LSR
		["lsr_a"] = func_noarg(0x4A),
		["lsr_dp"] = func_arg8(0x46),
		["lsr_dp_x"] = func_arg16(0x56),
		["lsr_mem"] = func_arg16(0x4E),
		["lsr_mem_x"] = func_arg16(0x5E),
		--MVN
		["mvn"] = func_call(0x54, function(s, tgt, src) s:__write8(tgt); s:__write8(src); end),
		--MVP
		["mvp"] = func_call(0x44, function(s, tgt, src) s:__write8(tgt); s:__write8(src); end),
		--NOP
		["nop"] = func_noarg(0xEA),
		--ORA
		["ora_dp"] = func_arg8(0x05),
		["ora_dp_ind"] = func_arg8(0x12),
		["ora_dp_ind_x"] = func_arg8(0x01),
		["ora_dp_ind_y"] = func_arg8(0x11),
		["ora_dp_ind_long"] = func_arg8(0x07),
		["ora_dp_ind_long_y"] = func_arg8(0x17),
		["ora_dp_x"] = func_arg8(0x15),
		["ora_imm"] = func_argA(0x09),
		["ora_mem"] = func_arg16(0x0D),
		["ora_mem_x"] = func_arg16(0x1D),
		["ora_mem_y"] = func_arg16(0x19),
		["ora_long"] = func_argL(0x0F),
		["ora_long_x"] = func_argL(0x1F),
		["ora_sr"] = func_argr8(0x03),
		["ora_sr_ind_y"] = func_argr8(0x13),
		--PEA
		["pea"] = func_arg16(0xF4),
		--PEI
		["pei"] = func_arg8(0xD4),
		--PER
		["per"] = func_call(0x62, function(self, label, fixup) self:__reflabel16(label, fixup or 0) end),
		--PHA
		["pha"] = func_noarg(0x48),
		--PHB
		["phb"] = func_noarg(0x8B),
		--PHD
		["phd"] = func_noarg(0x0B),
		--PHK
		["phk"] = func_noarg(0x4B),
		--PHP
		["php"] = func_noarg(0x08),
		--PHX
		["phx"] = func_noarg(0xDA),
		--PHY
		["phy"] = func_noarg(0x5A),
		--(PIC call).
		["pic_call"] = function(self, label)
			self:per(3);
			self:per(label, 1);
			self:rts();
		end,
		--PLA
		["pla"] = func_noarg(0x68),
		--PLB
		["plb"] = func_noarg(0xAB),
		--PLD
		["pld"] = func_noarg(0x2B),
		--PLP
		["plp"] = func_noarg(0x28),
		--PLX
		["plx"] = func_noarg(0xFA),
		--PLY
		["ply"] = func_noarg(0x7A),
		--REP
		["rep"] = func_call(0xC2, function(self, flg) self:__write8(flg); self:__clearflags(flg); end),
		--ROL
		["rol_a"] = func_noarg(0x2A),
		["rol_dp"] = func_arg8(0x26),
		["rol_dp_x"] = func_arg16(0x36),
		["rol_mem"] = func_arg16(0x2E),
		["rol_mem_x"] = func_arg16(0x3E),
		--ROR
		["ror_a"] = func_noarg(0x6A),
		["ror_dp"] = func_arg8(0x66),
		["ror_dp_x"] = func_arg16(0x76),
		["ror_mem"] = func_arg16(0x6E),
		["ror_mem_x"] = func_arg16(0x7E),
		--RTI
		["rti"] = func_noarg(0x40),
		--RTL
		["rtl"] = func_noarg(0x6B),
		--RTS
		["rts"] = func_noarg(0x60),
		--SBC
		["sbc_dp"] = func_arg8(0xE5),
		["sbc_dp_ind"] = func_arg8(0xF2),
		["sbc_dp_ind_x"] = func_arg8(0xE1),
		["sbc_dp_ind_y"] = func_arg8(0xF1),
		["sbc_dp_ind_long"] = func_arg8(0xE7),
		["sbc_dp_ind_long_y"] = func_arg8(0xF7),
		["sbc_dp_x"] = func_arg8(0xF5),
		["sbc_imm"] = func_argA(0xE9),
		["sbc_mem"] = func_arg16(0xED),
		["sbc_mem_x"] = func_arg16(0xFD),
		["sbc_mem_y"] = func_arg16(0xF9),
		["sbc_long"] = func_argL(0xEF),
		["sbc_long_x"] = func_argL(0xFF),
		["sbc_sr"] = func_argr8(0xE3),
		["sbc_sr_ind_y"] = func_argr8(0xF3),
		--SEC
		["sec"] = func_noarg(0x38),
		--SED
		["sed"] = func_noarg(0xF8),
		--SEI
		["sei"] = func_noarg(0x78),
		--SEP
		["sep"] = func_call(0xE2, function(self, flg) self:__write8(flg); self:__setflags(flg); end),
		--STA
		["sta_dp"] = func_arg8(0x85),
		["sta_dp_ind"] = func_arg8(0x92),
		["sta_dp_ind_x"] = func_arg8(0x81),
		["sta_dp_ind_y"] = func_arg8(0x91),
		["sta_dp_ind_long"] = func_arg8(0x87),
		["sta_dp_ind_long_y"] = func_arg8(0x97),
		["sta_dp_x"] = func_arg8(0x95),
		["sta_mem"] = func_arg16(0x8D),
		["sta_mem_x"] = func_arg16(0x9D),
		["sta_mem_y"] = func_arg16(0x99),
		["sta_long"] = func_argL(0x8F),
		["sta_long_x"] = func_argL(0x9F),
		["sta_sr"] = func_argr8(0x83),
		["sta_sr_ind_y"] = func_argr8(0x93),
		--STP
		["stp"] = func_noarg(0xDB),
		--STX
		["stx_dp"] = func_arg8(0x86),
		["stx_dp_y"] = func_arg8(0x96),
		["stx_mem"] = func_arg16(0x8E),
		["stx_mem_y"] = func_arg16(0x9E),
		--STY
		["sty_dp"] = func_arg8(0x84),
		["sty_dp_x"] = func_arg8(0x94),
		["sty_mem"] = func_arg16(0x8C),
		["sty_mem_x"] = func_arg16(0x9C),
		--STZ
		["stz_dp"] = func_arg8(0x64),
		["stz_dp_x"] = func_arg8(0x74),
		["stz_mem"] = func_arg16(0x9C),
		["stz_mem_x"] = func_arg16(0x9E),
		--TAX
		["tax"] = func_noarg(0xAA),
		--TAY
		["tay"] = func_noarg(0xA8),
		--TCD
		["tcd"] = func_noarg(0x5B),
		--TCS
		["tcs"] = func_noarg(0x1B),
		--TDC
		["tdc"] = func_noarg(0x7B),
		--TRB
		["trb_dp"] = func_arg8(0x14),
		["trb_mem"] = func_arg16(0x1C),
		--TSB
		["tsb_dp"] = func_arg8(0x04),
		["tsb_mem"] = func_arg16(0x0C),
		--TSC
		["tsc"] = func_noarg(0x3B),
		--TSX
		["tsx"] = func_noarg(0xBA),
		--TXA
		["txa"] = func_noarg(0x8A),
		--TXS
		["txs"] = func_noarg(0x9A),
		--TXY
		["txy"] = func_noarg(0x9B),
		--TYA
		["tya"] = func_noarg(0x98),
		--TYX
		["tyx"] = func_noarg(0xBB),
		--WAI
		["wai"] = func_noarg(0xCB),
		--WDM
		["wdm"] = func_arg8(0x42),
		--XBA
		["xba"] = func_noarg(0xEB),
		--XCE
		["xce"] = func_noarg(0xFB),
	};
end
