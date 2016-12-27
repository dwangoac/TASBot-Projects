tmp = "/tmp/img.tmp";
tmp_stage4 = "/tmp/stage4.tmp";
tmp_stage5 = "/tmp/stage5.tmp";

--s5size is the size of stage5. If there are lots of segments, it can be increased. You will get an error message
--if it is too small. The largest possible value is 1536.
--segment_headers = {["entrypoint"] = 0x7E8000, ["s5size"] = 512, ["segments"] = {
--	{["loadaddr"] = 0x7E8000, ["size"] = 512, ["fileoffset"] = 0, ["filename"] = "test-burninline.sfc"},
--	{["loadaddr"] = 0x7F0000, ["size"] = 65536, ["fileoffset"] = 0x8000, ["filename"] = "test-burninline.sfc"},
--	{["loadaddr"] = 0x7E9000, ["size"] = 512, ["fileoffset"] = 0x18000, ["filename"] = "test-burninline.sfc"},
--}}

dofile("library.lua");

write2le = function(v)
	return string.char(v%256)..string.char(math.floor(v/256));
end

write_payload = function(content, speed)
	local blocks = math.ceil(#content / (8 * speed));
	--Write actual data.
	for i = 0,blocks-1 do
		local block = string.sub(content, 8 * speed * i + 1, 8 * speed * (i + 1));
		while #block < 8 * speed do block = block .. string.byte(0); end
		for j = 0,speed-1 do
			local base = 8*(speed-j-1);	--Backwards!!!
			local a, b, c, d, e, f, g, h = string.byte(block, base+1, base+8);
			add_frame_scrambled(g, h, e, f, c, d, a, b, j == 0);
		end
	end
end

_make_movie = function(payload, speed)
	write_4218(tmp_stage4, {});
	write_rawcopy(tmp_stage5);
	--There is one blank frame before the data read starts.
	add_frame(0, 0, 0, 0, 0, 0, 0, 0, true);
	--The rest of stage5.
	write_payload(payload, speed)
	print("Done");
end

dofile("macroasm.lua");
dofile("stage4.lua");
dofile("stage5m.lua");
dofile("stage5p.lua");

stage5_baseaddr = 0x0200

make_stage4 = function(tmpfile, s5size)
	local out = io.open(tmpfile, "w");
	local base = 0x1900;
	out:write(string.format("0x%06x\n", base));
	--8 bytes of underrun.
	local s4code = stage4(snes_libasm(nil), stage5_baseaddr-8, stage5_baseaddr, s5size);
	for i=1,#s4code do
		out:write(string.format("0x%04x 0x%02x\n", base + i - 1, s4code[i]));
	end
	out:close();
end

make_stage5 = function(tmpfile, speed)
	local out = io.open(tmpfile, "wb");
	local s5code = stage5m(snes_libasm(stage5_baseaddr), stage5_baseaddr, speed);
	for i=1,#s5code do
		out:write(string.char(s5code[i]));
	end
	out:close();
end

make_movie = function(name)
	local s5size = 512;
	local speed = 75;
	--The remainder of Stage5 goes to cotents. The maximum size is speed*8, so 160 gives 1280 bytes.
	local s5pcode = stage5p(snes_libasm(stage5_baseaddr+S5M_CODE_LENGTH), stage5_baseaddr);
	contents="";
	print(string.format("#s5pcode=%d",#s5pcode));
	for i=1,#s5pcode do
		contents=contents..string.char(s5pcode[i]);
	end

	--Stage4 does NOT like payloads whose length is not multiple by 8. Also, add 8 bytes of room so headroom
	--does not mess things up.
	make_stage4(tmp_stage4, 8*math.ceil(S5M_CODE_LENGTH/8)+8);
	make_stage5(tmp_stage5, speed);
	_make_movie(contents, speed);
	--Two frames for delay.
	add_frame(0, 0, 0, 0, 0, 0, 0, 0, true);
	add_frame(0, 0, 0, 0, 0, 0, 0, 0, true);
	--Now, append video data if any.
	if name then
		file = io.open(name, "rb");
		local index = 0;
		local set8 = function(frame, port, controller, base, data)
			for i = 0,7 do
				frame:set_button(port, controller, base + i, bit.all(bit.lrshift(data, 7 - i),
					1) ~= 0);
			end
		end
		while true do
			local data = file:read(16);
			if not data then break; end
			local frame = movie.blank_frame();
			frame:set_button(0, 0, 0, (index % 261) == 0);
			set8(frame, 1, 0, 0, string.byte(string.sub(data, 1, 1)));
			set8(frame, 1, 0, 8, string.byte(string.sub(data, 2, 2)));
			set8(frame, 1, 1, 0, string.byte(string.sub(data, 3, 3)));
			set8(frame, 1, 1, 8, string.byte(string.sub(data, 4, 4)));
			set8(frame, 2, 0, 0, string.byte(string.sub(data, 9, 9)));
			set8(frame, 2, 0, 8, string.byte(string.sub(data, 10, 10)));
			set8(frame, 2, 1, 0, string.byte(string.sub(data, 11, 11)));
			set8(frame, 2, 1, 8, string.byte(string.sub(data, 12, 12)));
			movie.append_frame(frame);
			index = index + 1;
		end
	end
end
