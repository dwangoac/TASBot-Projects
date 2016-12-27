lo = function(a) return bit.band(a, 0xFF); end
mid = function(a) return bit.band(bit.lrshift(a, 8), 0xFF); end
hi = function(a) return bit.band(bit.lrshift(a, 16), 0xFF); end

add_frame=function(a, b, c, d, e, f, g, h, sync)
	local set8 = function(obj, port, controller, index, val)
		for i=0,7 do obj:set_button(port, controller, index + i, bit.test_all(bit.lshift(val, i), 128)); end
	end
	local frame = movie.blank_frame();
	frame:set_button(0, 0, 0, sync);
	set8(frame, 1, 0, 0, b);
	set8(frame, 1, 0, 8, a);
	set8(frame, 1, 1, 0, f);
	set8(frame, 1, 1, 8, e);
	set8(frame, 2, 0, 0, d);
	set8(frame, 2, 0, 8, c);
	set8(frame, 2, 1, 0, h);
	set8(frame, 2, 1, 8, g);
	movie.append_frame(frame);
end


write_byte_scrambled=function(frame, base, port, byte)
	for i=0,3 do
		frame:set_button(port, 0, base + i, bit.test_all(byte, 64))
		frame:set_button(port, 1, base + i, bit.test_all(byte, 128))
		byte = bit.lshift(byte, 2);
	end
end

--000000ab
--0000ab00
--00ab0000
--ab000000
----------
--00110101
--ab111000


write_4218 = function(filename, options)
	--Prelude suitable for exploiting some bug in LoZ.
	add_frame(0x42, 0x4C, 0xEA, 0x64, 0x12, 0xCB, 0x00, 0x00, true);
	add_frame(0xC2, 0x20, 0xEA, 0x64, 0x12, 0xCB, 0x80, 0xF8, true);
	--Parse the input file.
	local writes = {};
	local writes_addr = {};
	local jump_address = nil;
	local file, err = io.open(filename);
	if not file then error(err); end
	for i in file:lines() do
		if i == "" then
		elseif not jump_address then
			jump_address = tonumber(i);
		else
			local a, b = string.match(i, "(%w+)%s+(%w+)");
			a = tonumber(a);
			b = tonumber(b);
			writes[a] = b;
		end
	end
	--Make indirect sort of the data by address.
	for k,v in pairs(writes) do table.insert(writes_addr, k); end
	table.sort(writes_addr);
	--Write the data, either as paired writes or unpaired writes.
	local i = 1;
	while i < #writes_addr do
		local addr = writes_addr[i];
		local naddr = writes_addr[i+1] or -1;
		local data_lo = writes[addr];
		local data_hi = writes[naddr] or 0;
		if naddr == addr + 1 then
			--Paired write.
			add_frame(0xA9, data_lo, data_hi, 0x64, 0x12, 0xCB, 0x80, 0xF8, true);
			add_frame(0x8D, lo(addr), mid(addr), 0x64, 0x12, 0xCB, 0x80, 0xF8, true);
			i = i + 2;
		else
			--Unpaired write.
			add_frame(0xA9, data_lo, 0x00, 0x64, stz_addr, 0xCB, 0x80, 0xF8, true);
			add_frame(0x8D, lo(addr), mid(addr), 0x64, stz_addr, 0xCB, 0x80, 0xF8, true);
			print(string.format("Warning, unpaired write, will write zero to %04x!", addr+1));
			i = i + 1;
		end
	end
	--We restore 8-bit mode, since payloads probably assume that.
	add_frame(0xE2, 0x30, 0x5C, lo(jump_address), mid(jump_address), hi(jump_address), 0x80, 0xF8, true);
	file:close();
end

write_rawcopy = function(filename)
	local file, err = io.open(filename);
	if not file then error(err); end
	while true do
		local data = file:read(8);
		if not data then break; end
		while #data < 8 do data = data .. string.char(0); end
		local a, b, c, d, e, f, g, h = string.byte(data, 1, 8);
		add_frame(a, b, c, d, e, f, g, h, true);
	end
	file:close();
end

write_segment = function(filename, offset, base, length, speed)
	speed = speed or 8;
	local file, err = io.open(filename);
	if not file then error(err); end
	file:seek("set", offset);
	local blocks = math.ceil(length / (8 * speed));
	--Write actual data.
	for i = 0,blocks-1 do
		for j = 0,speed-1 do
			local data = file:read(8);
			if data == nil then data = string.char(0, 0, 0, 0, 0, 0, 0, 0); end
			while #data < 8 do data = data .. string.char(0); end
			local a, b, c, d, e, f, g, h = string.byte(data, 1, 8);
			add_frame(a, b, c, d, e, f, g, h, j == 0);
		end
	end
	file:close();
end

add_frame_scrambled=function(a, b, c, d, e, f, g, h, sync)
	local frame = movie.blank_frame();
	frame:set_button(0, 0, 0, sync);
	write_byte_scrambled(frame, 0, 1, a);
	write_byte_scrambled(frame, 0, 2, bit.bxor(b, 0xAC));
	write_byte_scrambled(frame, 4, 1, c);
	write_byte_scrambled(frame, 4, 2, bit.bxor(d, 0xAC));
	write_byte_scrambled(frame, 8, 1, e);
	write_byte_scrambled(frame, 8, 2, bit.bxor(f, 0xAC));
	write_byte_scrambled(frame, 12, 1, g);
	write_byte_scrambled(frame, 12, 2, bit.bxor(h, 0xAC));
	movie.append_frame(frame);
end
