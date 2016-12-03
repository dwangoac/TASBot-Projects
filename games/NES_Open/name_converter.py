import sys
import re

if len(sys.argv) < 2:
  sys.stderr.write('Usage: ' + sys.argv[0] + ' <string>\n\n')
  sys.exit(0)

# Sanatize the input string
name_string = sys.argv[1].lower()
bad_chars = re.search("([^a-z. ])", name_string)
if (bad_chars != None):
  print "Illegal character: " + bad_chars.group(0);

# Positions of each character based on how far it is from the start point (A)  
text_grid = {"a": [0,0], "b": [1,0], "c": [2,0], "d": [3,0], "e": [4,0], "f": [5,0], "g": [6,0], "h": [7,0],
             "i": [0,1], "j": [1,1], "k": [2,1], "l": [3,1], "m": [4,1], "n": [5,1], "o": [6,1], "p": [7,1],
             "q": [0,2], "r": [1,2], "s": [2,2], "t": [3,2], "u": [4,2], "v": [5,2], "w": [6,2], "x": [7,2],
             "y": [0,3], "z": [1,3], ".": [2,3], " ": [3,3], "<": [4,3], ">": [5,3], "end1": [6,3], "end2": [7,3]}

# List to hold the final input
final_output = []

def get_delta_movement(cur_pos, next_pos):
  # Compute the simple distance
  delta = [next_pos[0] - cur_pos[0], next_pos[1] - cur_pos[1]]
  
  # Handle wrapping around the screen
  
  # Negative is left, positive is right
  if (abs(delta[0]) > 4):
    if (delta[0] < 0):
      delta[0] = 8 + delta[0]
    else:
      delta[0] = delta[0] - 8
  
  # Negative is up, positive is down
  if (abs(delta[1]) > 2):
    if (delta[1] < 0):
      delta[1] = 4 + delta[1]
    else:
      delta[1] = delta[1] - 4
      
  return delta
  
def generate_input_from_delta(movement):
    global final_output
    
    # To be safe, add 2 blank frames before doing a movement
    # This prevents problems with repeating the same input (but does make it suboptimal)
    
    # Move horizontally first
    for i in range (0,abs(movement[0])):
        final_output.append("")
        final_output.append("")
        if (movement[0] < 0):
            final_output.append("L")
        else:
            final_output.append("R")
    
    # Then move vertically
    for i in range (0,abs(movement[1])):
        final_output.append("")
        final_output.append("")
        if (movement[1] < 0):
            final_output.append("U")
        else:
            final_output.append("D")
             
def input_single_name(desired_name):   
    global text_grid, final_output
    
    # We always start on A tile
    cur_pos = [0,0]
    
    for i in range(0,10):
        next_pos = text_grid[desired_name[i:i+1]]

        # Determine how far to move to the next letter
        delta = get_delta_movement(cur_pos, next_pos)
        
        # Generate the input for that movement
        generate_input_from_delta(delta)
        
        # Press A to confirm the letter
        final_output.append("A")
        
        # Wait 12 frames after pressing A to finish the animation
        for i in range (0,12):
            final_output.append("")
        
        cur_pos = next_pos
        
    # Calculate distance to end tile
    delta_end1 = get_delta_movement(cur_pos, text_grid["end1"])
    delta_end2 = get_delta_movement(cur_pos, text_grid["end2"])
    
    # Figure out which spot is closer
    distance1 = abs(delta_end1[0]) + abs(delta_end1[1])
    distance2 = abs(delta_end2[0]) + abs(delta_end2[1])
    
    # Generate input to that space
    if (distance1 > distance2):
        generate_input_from_delta(delta_end2)
    else:
        generate_input_from_delta(delta_end1)
    
    # Press A to confirm the name
    final_output.append("A")
      
  
for name_index in range(0,36):
    current_name = name_string[name_index*10:name_index*10+10]
    if (len(current_name) == 0):
        break
    padded_name = current_name.ljust(10)

    # We are on the roster screen
    # Press A to get to the name entry screen
    final_output.append("A")

    # Now wait 55 frames
    for i in range (0,55):
        final_output.append("")
       
    # Enter the name
    input_single_name(padded_name)

    # wait 96 frames to exit name screen
    for i in range (0,96):
        final_output.append("")
    
    if (len(current_name) < 10):
        break

    # Press A to confirm we want to enter another name
    final_output.append("A")
    
    # wait 63 frames to get back to the roster screen
    for i in range (0,63):
        final_output.append("")
        
    # Go to the next name
    if ((name_index % 2) == 1):
      final_output.append("D")
      final_output.append("L")
    else:
      final_output.append("R")
      
      
print "TAS " + str(len(final_output))
for i in range (0,len(final_output)):
  print final_output[i]