--mapping = [0, 2, 1, 3, 4, 6, 5, 7];   -- p1d0, p1d1, p2d0, p2d1
--mapping = {0, 1, 2, 3, 4, 5, 6, 7};  -- p1i1, p1i0, p2i1, p2i0

--possible bug: you may need to add a frame to the beginning of input

joymap = {}
joymap["A"]         = 7
joymap["B"]         = 6
joymap["Select"]    = 5
joymap["Start"]     = 4
joymap["Up"]        = 3
joymap["Down"]      = 2
joymap["Left"]      = 1
joymap["Right"]     = 0

file_suffix = "frame.r08"
    

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

function fileclose()
    dumpfile:close()
    dumpfile = nil
    print("dumping completed")
end

function dumper()
    if dumpfile then
        if movie.mode() ~= "PLAY" and dumpfile then
            fileclose()
        elseif not emu.islagged() then
            local p1 = 0
            local p2 = 0
            local input = movie.getinput(emu.framecount() - 1)

            pollcount = pollcount + 1

            -- copy input data to bit data
            for k, v in pairs(input) do
                local plyr   = string.sub(k, 1, 2)
                local id     = string.sub(k, 4, string.len(k))
                local mod    = joymap[id]

                if v == true then
                    -- print(k)
                    if plyr == "P1" then p1 = bit.bor(p1, bit.lshift(1, mod)); end
                    if plyr == "P2" then p2 = bit.bor(p2, bit.lshift(1, mod)); end
                end
            end

            -- print(p1..","..p2) -- print out raw input data

            -- write bit data
            dumpfile:write(string.char(bit.band(p1, 0xff), bit.band(p2, 0xff)))

            if pollcount % 1024 == 0 then
                print("wrote "..tostring(pollcount).." frames")
            end
        end
    end
end

event.onframestart(dumper)
event.onexit(fileclose)

while true do
    -- event-based
    emu.frameadvance()
end