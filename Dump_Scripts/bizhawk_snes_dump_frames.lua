--mapping = [0, 2, 1, 3, 4, 6, 5, 7];   -- p1d0, p1d1, p2d0, p2d1
--mapping = {0, 1, 2, 3, 4, 5, 6, 7};  -- p1i1, p1i0, p2i1, p2i0

--possible bug: you may need to add a frame to the beginning of input

joymap = {}
joymap["B"]         = 0
joymap["Y"]         = 1
joymap["Select"]    = 2
joymap["Start"]     = 3
joymap["Up"]        = 4
joymap["Down"]      = 5
joymap["Left"]      = 6
joymap["Right"]     = 7
joymap["A"]         = 8
joymap["X"]         = 9
joymap["L"]         = 10
joymap["R"]         = 11
joymap["et"]     = 99 -- Reset
joymap["er"]     = 98 -- Power


file_suffix = "frame.r16m"
    

--[[
while true do
    print(movie.getinput(emu.framecount()))
    emu.frameadvance()
end
]]

-- a movie needs to be playing to do anything
if movie.mode() ~= "PLAY" then
    error("error movie not playing - run this script after movie started and paused")
end

-- get r16m filename
_, _, path, filename, ext = string.find(movie.filename(), "(.-)([^\\/]-%.?)([^%.\\/]*)$")

-- open for writing
fh, err = io.open(path..filename..file_suffix, "wb")
if not fh then
    error("error opening output file: "..err)
end
print("dumping to "..path..filename..file_suffix)

dumpfile = fh
pollcount = 0
frame = 0

fh = nil
err = nil

while true do
    if dumpfile then
        if movie.mode() ~= "PLAY" and dumpfile then
            dumpfile:close()
            dumpfile = nil
            print("dumping completed")
        elseif not emu.islagged() then
            local p1 = 0
            local p2 = 0
            local p3 = 0
            local p4 = 0
            local p5 = 0
            local p6 = 0
            local p7 = 0
            local p8 = 0
            local p1a = 0
            local p2a = 0
            local p3a = 0
            local p4a = 0
            local p5a = 0
            local p6a = 0
            local p7a = 0
            local p8a = 0
            local input = movie.getinput(emu.framecount())
            local bytenum

            pollcount = pollcount + 1

            -- copy input data to bit data
            for k, v in pairs(input) do
                local player = string.sub(k, 1, 2)
                local id     = string.sub(k, 4, string.len(k))
                local mod    = joymap[id]

                if mod < 8 then bytenum = 1; elseif mod < 16 then bytenum = 2; end
              
                if input[k] == true then
                    if bytenum == 1 then
                        if player == "P1" then p1 = bit.bor(p1, bit.lshift(1, 7 - mod)); end
                        if player == "P2" then p2 = bit.bor(p2, bit.lshift(1, 7 - mod)); end
                        if player == "P3" then p2 = bit.bor(p3, bit.lshift(1, 7 - mod)); end
                        if player == "P4" then p2 = bit.bor(p4, bit.lshift(1, 7 - mod)); end
                        if player == "P5" then p2 = bit.bor(p5, bit.lshift(1, 7 - mod)); end
                        if player == "P6" then p2 = bit.bor(p6, bit.lshift(1, 7 - mod)); end
                        if player == "P7" then p2 = bit.bor(p7, bit.lshift(1, 7 - mod)); end
                        if player == "P8" then p2 = bit.bor(p8, bit.lshift(1, 7 - mod)); end
                    end
                    if bytenum == 2 then
                        mod = mod - 8
                        if player == "P1" then p1a = bit.bor(p1a, bit.lshift(1, 7 - mod)); end
                        if player == "P2" then p2a = bit.bor(p2a, bit.lshift(1, 7 - mod)); end
                        if player == "P3" then p2a = bit.bor(p3a, bit.lshift(1, 7 - mod)); end
                        if player == "P4" then p2a = bit.bor(p4a, bit.lshift(1, 7 - mod)); end
                        if player == "P5" then p2a = bit.bor(p5a, bit.lshift(1, 7 - mod)); end
                        if player == "P6" then p2a = bit.bor(p6a, bit.lshift(1, 7 - mod)); end
                        if player == "P7" then p2a = bit.bor(p7a, bit.lshift(1, 7 - mod)); end
                        if player == "P8" then p2a = bit.bor(p8a, bit.lshift(1, 7 - mod)); end
                    end
                end
            end

            -- write bit data
            dumpfile:write(string.char(
                bit.band(p1, 0xff),
                bit.band(p1a, 0xff),
                bit.band(p2, 0xff),
                bit.band(p2a, 0xff),
                bit.band(p3, 0xff),
                bit.band(p3a, 0xff),
                bit.band(p4, 0xff),
                bit.band(p4a, 0xff),
                bit.band(p5, 0xff),
                bit.band(p5a, 0xff),
                bit.band(p6, 0xff),
                bit.band(p6a, 0xff),
                bit.band(p7, 0xff),
                bit.band(p7a, 0xff),
                bit.band(p8, 0xff),
                bit.band(p8a, 0xff)
            ));

            if pollcount % 1024 == 0 then
                print("wrote "..tostring(pollcount).." frames")
            end
        end
    end

    emu.frameadvance()
end