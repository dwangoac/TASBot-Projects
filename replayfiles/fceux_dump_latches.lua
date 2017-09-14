-- Load movie then run this; when the movie is over, ADVANCE ONE MORE FRAME, then STOP!
-- Running this first then loading the movie may run into unexpected problems in this version.

local movie_loaded = false;     -- This tells if the program has initialized for the movie
local movie_filename = "";      -- Will hold the movie file name (.fm2)
local output_filename = "";     -- Will hold the output file name (.r08)
fhnd = 0;                       -- For ouputing to the file

prev_input = {};

frame = 0;
latch_cycle = 1;
latch_writes_per_poll = 2;      -- some games (SMB3) that latch independently for
                                -- player 1 and player 2 (?) need to be set to 4

----

function update_bitfile()
    -- Order of buttons: RLDUTSBA
    local player = {0, 0};
    local jp = {};

    -- there are two writes per latch (write 0x4016, 1 and write 0x4016, 2)
    -- thus we need to ignore every other write.
    if (latch_cycle >= latch_writes_per_poll) then
    	latch_cycle = 1;


    	jp[1] = joypad.get(1);
    	jp[2] = joypad.get(2);

	for i = 1, 2 do
	        if (jp[i].right == true)  then player[i] = OR(player[i],BIT(0)); end;
	        if (jp[i].left == true)   then player[i] = OR(player[i],BIT(1)); end;
	        if (jp[i].down == true)   then player[i] = OR(player[i],BIT(2)); end;
	        if (jp[i].up == true)     then player[i] = OR(player[i],BIT(3)); end;
	        if (jp[i].start == true)  then player[i] = OR(player[i],BIT(4)); end;
	        if (jp[i].select == true) then player[i] = OR(player[i],BIT(5)); end;
	        if (jp[i].B == true)      then player[i] = OR(player[i],BIT(6)); end;
	        if (jp[i].A == true)      then player[i] = OR(player[i],BIT(7)); end;
	end

	fhnd:write(string.char(player[1]));
	fhnd:write(string.char(player[2]));

	-- print('l');
    else
    	latch_cycle = latch_cycle + 1;
    end
end

emu.speedmode("turbo")

while (true) do
    -- If a movie is loaded into the emulator
    if (movie.active() == true) then
        -- When a movie is loaded for the first time, we need to do some setup
        if (movie_loaded == false) then
            -- First, restart the movie at the beginning
            movie.playbeginning();
            
            -- Lets make up the output filename
            -- Take the video name, remove the .fm2 and replace with .r08
            movie_filename = movie.getname();
            output_filename = string.sub(movie_filename, 0, string.len(movie_filename)-4) .. ".polls.r08";
            
            -- Print it out for debugging
            print("Writing to " .. output_filename .. "...");
            
            -- Setup the file handle to write to it, open in binary mode
            fhnd = io.open(output_filename, "wb+");
            
            -- Now we are ready to go.
            movie_loaded = true;

            -- when latch is written, we write the player data that would be sent
        	memory.registerwrite(0x4016, 1, update_bitfile);
        end
        
        -- why not keep track of frames? maybe use for debugging? ...
        if (movie.framecount() > 1) then
            frame = frame + 1;
        end
    else
        -- If the movie has ended, then our work here is done. Clean up
        if (movie_loaded == true) then
            fhnd:close();
            
            print("DONE");
            movie_loaded = false;
            frame = 0;
        end
    end
    
    -- Mandatory
    FCEU.frameadvance();
end