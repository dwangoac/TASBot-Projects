-- Run this with the movie; when the movie is over, ADVANCE TWO MORE FRAMES, then STOP!

local movie_loaded = false;     -- This tells if the program has initialized for the movie
local movie_filename = "";      -- Will hold the movie file name (.fm2)
local output_filename = "";     -- Will hold the output file name (.r08)
local fhnd;                     -- For ouputing to the file

local lagged = false;
local prev_input = {};

local skip_lag = true;
local frame = 0;


function update_bitfile(fhnd, prev_input)
    -- Order of buttons: RLDUTSBA
    local player = {0, 0};

    for i = 1, 2 do
        if (prev_input[i].right == true)  then player[i] = OR(player[i],BIT(0)); end;
        if (prev_input[i].left == true)   then player[i] = OR(player[i],BIT(1)); end;
        if (prev_input[i].down == true)   then player[i] = OR(player[i],BIT(2)); end;
        if (prev_input[i].up == true)     then player[i] = OR(player[i],BIT(3)); end;
        if (prev_input[i].start == true)  then player[i] = OR(player[i],BIT(4)); end;
        if (prev_input[i].select == true) then player[i] = OR(player[i],BIT(5)); end;
        if (prev_input[i].B == true)      then player[i] = OR(player[i],BIT(6)); end;
        if (prev_input[i].A == true)      then player[i] = OR(player[i],BIT(7)); end;
    end

    fhnd:write(string.char(player[1]));
    fhnd:write(string.char(player[2]));
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
            output_filename = string.sub(movie_filename, 0, string.len(movie_filename)-4) .. ".frame.r08";
            
            -- Print it out for debugging
            print("Writing to " .. output_filename .. "...");
            
            -- Setup the file handle to write to it, open in binary mode
            fhnd = io.open(output_filename, "wb+");

            if (fhnd == nil) then
                print("Error opening movie file!")
            end
            
            -- Now we are ready to go.
            movie_loaded = true;
        end
        
        -- There are some issues with starting this code, so we pretend to skip the first frame
        -- I believe the problem is because when the video is loaded it is at frame 0 which has no input
        -- We need to skip this frame, look for lag, then start watching for input
        if (movie.framecount() > 1) then
            -- We need to skip any lag frames and only output frames where the console is looking for input
            if (lagged == true) then
                fhnd:write("");
            else
                frame = frame + 1;
                
                -- Convert the buttons from the movie into byte format and store it to the file
                update_bitfile(fhnd, prev_input);
            end
        end
        
        -- Check for lag frames and record it
        if (FCEU.lagged() == true) then
            lagged = true;
        else
            lagged = false;
        end
        
        -- Also read the input for the previous frame
        prev_input[1] = joypad.get(1);
        prev_input[2] = joypad.get(2);
          
    else
        -- If the movie has ended, then our work here is done. Clean up
        if (movie_loaded == true) then
            -- update_bitfile(fhnd, prev_input);

            fhnd:close();
            
            print("DONE");
            movie_loaded = false;
            frame = 0;
        end
    end
    
    -- Mandatory
    FCEU.frameadvance();
end