for i=1,64 do
	local frame = movie.blank_frame();
	frame:set_button(0, 0, 0, false);
	for j=0,15 do
		frame:set_button(1, 0, j, random.boolean());
		frame:set_button(1, 1, j, random.boolean());
		frame:set_button(2, 0, j, random.boolean());
		frame:set_button(2, 1, j, random.boolean());
	end
	movie.append_frame(frame);
end

for i=0,99999 do
	local frame = movie.blank_frame();
	frame:set_button(0, 0, 0, (i % 243) == 0);
	for j=0,15 do
		frame:set_button(1, 0, j, random.boolean());
		frame:set_button(1, 1, j, random.boolean());
		frame:set_button(2, 0, j, random.boolean());
		frame:set_button(2, 1, j, random.boolean());
	end
	movie.append_frame(frame);
end
