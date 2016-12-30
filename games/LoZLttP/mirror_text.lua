--debug display control
local show_debug = true
local show_dma_debug = false
local active_screen = sprite_screen
local opacity = 0x80
local fg_color = 0x00FFFFFF
local bg_color = 0x00000000
local x_padding = -400
local y_padding = 0

--Add transparency to a color with default opacity
local function trans(color)
	return color + (opacity * 0x1000000)
end

local function text(x, y, message)
	gui.text(x+x_padding, y+y_padding, message, trans(fg_color), trans(bg_color))
end

function on_paint(sync)
	ports = string.format(
				"4016: %02X\n" ..
				"4017: %02X\n" ..
				
				"4213: %02X\n" ..
				
				"4218: %02X\n" ..
				"4219: %02X\n" ..
				"421A: %02X\n" ..
				"421B: %02X\n" ..
				"421C: %02X\n" ..
				"421D: %02X\n" ..
				"421E: %02X\n" ..
				"421F: %02X", 
				
				memory2.BUS:read(0x4016), 
				memory2.BUS:read(0x4017), 
				
				memory2.BUS:read(0x4213), 
				
				memory2.BUS:read(0x4218), 
				memory2.BUS:read(0x4219), 
				memory2.BUS:read(0x421A), 
				memory2.BUS:read(0x421B), 
				memory2.BUS:read(0x421C), 
				memory2.BUS:read(0x421D), 
				memory2.BUS:read(0x421E), 
				memory2.BUS:read(0x421F)
			)
			
	text(0, 0, ports)
end


local function toBits(input_array)
	local controllers = {}
	for i=0, 3 do
		local buttons = {}
		local input = input_array[i*2+1]+input_array[i*2+2]*256
		for k=0, 15 do
			buttons[16-k]=input%2
			input = bit.lrshift(input, 1)
		end
		table.insert(controllers, buttons)
	end
	
	controllers[2], controllers[3] = controllers[3], controllers[2]
	
	return controllers
end

--local start = 6418
local start = 14767
local inputs = {}

local stop = 0xDB
local nop = 0xEA
local wai = 0xCB

local function set_frame(i)
	table.insert(inputs, toBits({i, i, i, i, i, i, i, i}))
end

local function gen_id_frames(count)
	for i=1, count do
		set_frame(i)
	end
end

local function lda_sta(value, dest)
	local d_1 = bit.lrshift(dest, 8)
	local d_2 = bit.band(dest, 0xFF)
	
	local v_1 = bit.lrshift(value, 8)
	local v_2 = bit.band(value, 0xFF)
	
	table.insert(inputs, toBits({0xA9, v_2, v_1, 0xc6, 0x12, wai, 0x80, 0xF8}))
	table.insert(inputs, toBits({0x8D, d_2, d_1, 0xc6, 0x12, wai, 0x80, 0xF8}))
end

local function gen_input()
	table.insert(inputs, toBits({0xC2, 0x30, 0x64, 0x11, 0xEA, 0xCB, 0x80, 0xF8}))
	
	-- code stage (one per line): Warp, Message (x3), stop, text payload
	code = {  0x30E2, 0x0FA9, 0x1385, 0x008D, 0xA921, 0x8D08, 0x2140, 0xFF22, 0x02B1, 0x1264, 0xA5CB, 0xF0B0, 0x8002, 0xEAF3,
		  0xF09C, 0x9C1C, 0x1CF1, 0xBAA9, 0x1185, 0xC08F, 0x7F71, 0x07A9, 0xC18F, 0x7F71, 0x7EA9, 0xC28F, 0x7F71, 0x4022, 0x0EC4, 0x1264, 0xA5CB, 0xD011, 0xEADB,
		  0xF09C, 0x9C1C, 0x1CF1, 0xE4A9, 0x1185, 0xC08F, 0x7F71, 0x07A9, 0xC18F, 0x7F71, 0x7EA9, 0xC28F, 0x7F71, 0x4022, 0x0EC4, 0x1264, 0xA5CB, 0xD011, 0xEADB,
		  0xF09C, 0x9C1C, 0x1CF1, 0x0EA9, 0x1185, 0xC08F, 0x7F71, 0x08A9, 0xC18F, 0x7F71, 0x7EA9, 0xC28F, 0x7F71, 0x4022, 0x0EC4, 0x1264, 0xA5CB, 0xD011, 0xEADB,
		  0x108D, 0xDB50,
		  0x047A, 0x1A03, 0x2730, 0x2859, 0x591F, 0x212D, 0x591E, 0x221F, 0x1A27, 0x5925, 0x1A1D, 0x4332, 0x3A75, 0x2159, 0x2E28, 0x2C2B, 0x2B59, 0x261E, 0x221A, 0x3E27, 0x7F7F,
		  0x127A, 0x1E03, 0x1A25, 0x2232, 0x2027, 0x1259, 0x2B2D, 0x1A1E, 0x4326, 0x4343, 0x4343, 0x4343, 0x4343, 0x4343, 0x4343, 0x7543, 0x4343, 0x4343, 0x4343, 0x4343, 0x437F,
		  0x047A, 0x1A03, 0x2730, 0x2859, 0x591F, 0x212D, 0x591E, 0x221F, 0x1A27, 0x5925, 0x1A1D, 0x4332, 0x3B75, 0x2159, 0x2E28, 0x2C2B, 0x2B59, 0x261E, 0x221A, 0x3E27, 0x7F7F}
	for i=1, #code do
		lda_sta(code[i], 0x0728 + i*2 - 2)
	end
  
	table.insert(inputs, toBits({0x5C, 0x28, 0x07, 0x00, 0xEA, 0xEA, 0x80, 0xF8}))
	set_frame(stop)

	--detector frames
	gen_id_frames(15)
end

gen_input()

function on_input()
	local index = movie.currentframe()-start
	if index>=0 and index<#inputs then
		local b = inputs[index+1]
		for c=0, 3 do
			for i=0, 15 do
				--print(b[c+1][i+1])
				input.set2(bit.lrshift(c, 1)+1, c%2, i, b[c+1][i+1])
				input.set2(bit.lrshift(c, 1)+1, c%2+2, i, b[c+1][i+1])
			end
		end
	end
end
