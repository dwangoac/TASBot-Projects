#include <iostream>
#include <fstream>
#include <vector>
#include "../process_image_stream/process_image/data_sizes.h"
#include "../process_image_stream/process_image/trans.h"

// Screen area size
#define SSIZE_X 32
#define SSIZE_Y 28

using namespace std;

typedef struct _tile_data
{
    uint8 pixels[8];
} tile_data;

bool process_image(string filename, vector<tile_data> &tile_set)
{
    uint8 * image_data;
    image_data = new unsigned char[SSIZE_X * SSIZE_Y * 8 * 8 * 3];

    // Read in file 1
    ifstream image_file(filename.c_str(), ios::in | ios::binary | ios::ate);
    if (image_file.is_open())
    {
      streampos image_size;
      image_size = image_file.tellg();

      if (image_size != SSIZE_X * SSIZE_Y * 8 * 8 * 3)
      {
          cout << "Image wrong size (expected " << (SSIZE_X * SSIZE_Y * 8 * 8 * 3) << "): " << filename << endl;
          return false;
      }
      
      image_file.seekg(0, ios::beg);
      image_file.read((char *)image_data, image_size);
      image_file.close();
    }
    else
    {
        cout << "Unable to open " << filename << endl;
        return false;
    }

    // Loop through tiles
    // check current list of tiles
    // make new entry if needed
    // add to tile list
    for (int tile_y = 0; tile_y < SSIZE_Y; tile_y++)
    {
        for (int tile_x = SSIZE_X - 1; tile_x >= 0; tile_x--)
        {
            uint8 cur_tile[8];
            for (int pixel_y = 0; pixel_y < 8; pixel_y++)
            {
                cur_tile[pixel_y] = 0;
                for (int pixel_x = 0; pixel_x < 8; pixel_x++)
                { 
                    // Check if this pixel is a 0 or a 1
                    // add to array
                    int image_offset = ((tile_y * 8) + pixel_y) * SSIZE_X * 8 * 3 + ((tile_x * 8) + pixel_x) * 3;
                    if (image_data[image_offset] != 0)
                    {
                        cur_tile[pixel_y] |= 1 << (7-pixel_x);
                    }
                }
            }
            
            bool found = false;
            int matched_tile = -1;
            // Search for the tile in the vector
            for (int i = 0; i < tile_set.size(); i++)
            {
                int match_count = 0;
                for (int row = 0; row < 8; row++)
                {
                    if (cur_tile[row] == tile_set[i].pixels[row])
                    {
                        match_count++;
                    }
                }
                
                if (match_count == 8)
                {
                    found = true;
                    matched_tile = i;
                    
                    break;
                }
            }
            
            if (!found)
            {
                tile_data new_tile;
                for (int i = 0; i < 8; i++)
                {
                    new_tile.pixels[i] = cur_tile[i];
                }
                
                tile_set.push_back(new_tile);
                matched_tile = tile_set.size() - 1;
            }
        }
    }

}

void tilize_image(string filename, vector<tile_data> &tile_set, int tile_offset, uint8 * output)
{
    uint8 * image_data;
    image_data = new unsigned char[SSIZE_X * SSIZE_Y * 8 * 8 * 3];

    // Read in file 1
    ifstream image_file(filename.c_str(), ios::in | ios::binary | ios::ate);
    if (image_file.is_open())
    {
      streampos image_size;
      image_size = image_file.tellg();

      if (image_size != SSIZE_X * SSIZE_Y * 8 * 8 * 3)
      {
          cout << "Image wrong size (expected " << (SSIZE_X * SSIZE_Y * 8 * 8 * 3) << "): " << filename << endl;
          return;
      }
      
      image_file.seekg(0, ios::beg);
      image_file.read((char *)image_data, image_size);
      image_file.close();
    }
    else
    {
        cout << "Unable to open " << filename << endl;
        return;
    }

    for (int tile_y = 0; tile_y < SSIZE_Y; tile_y++)
    {
        for (int tile_x = SSIZE_X - 1; tile_x >= 0; tile_x--)
        {
            uint8 cur_tile[8];
            for (int pixel_y = 0; pixel_y < 8; pixel_y++)
            {
                cur_tile[pixel_y] = 0;
                for (int pixel_x = 0; pixel_x < 8; pixel_x++)
                { 
                    // Check if this pixel is a 0 or a 1
                    // add to array
                    int image_offset = ((tile_y * 8) + pixel_y) * SSIZE_X * 8 * 3 + ((tile_x * 8) + pixel_x) * 3;
                    if (image_data[image_offset] != 0)
                    {
                        cur_tile[pixel_y] |= 1 << (7-pixel_x);
                    }
                }
            }
            
            bool found = false;
            int matched_tile = -1;
            // Search for the tile in the vector
            for (int i = 0; i < tile_set.size(); i++)
            {
                int match_count = 0;
                for (int row = 0; row < 8; row++)
                {
                    if (cur_tile[row] == tile_set[i].pixels[row])
                    {
                        match_count++;
                    }
                }
                
                if (match_count == 8)
                {
                    found = true;
                    matched_tile = i;
                    
                    break;
                }
            }
            
            if (!found)
            {
                cout << "Error: cannot file matching tile" << endl;
                return;
            }
            
            matched_tile += tile_offset;
            
            output[tile_y * SSIZE_X * 2 + tile_x * 2 + 0] = matched_tile & 0xFF;
            output[tile_y * SSIZE_X * 2 + tile_x * 2 + 1] = (matched_tile >> 8) & 0x0F;
        }
    }

}

int main ()
{
    // Palette: 0-254 = black, 255 = white
    uint8 palette[256*3];
    for (int i = 0; i < 256; i++)
    {
        palette[i*3 + 0] = 0;
        palette[i*3 + 1] = 0;
        palette[i*3 + 2] = 0;
    }
    palette[255*3 + 0] = 255;
    palette[255*3 + 1] = 255;
    palette[255*3 + 2] = 255;
  
    // Send palette, switch to high tile map
    trans_palette(palette, true);    
  
  
    // Convert images to tiles
    vector<tile_data> tile_set;
    process_image("images/skhype_logo000.rgb", tile_set);
    process_image("images/skhype_logo001.rgb", tile_set);
    process_image("images/skhype_logo002.rgb", tile_set);
    process_image("images/skhype_logo003.rgb", tile_set);
    process_image("images/skhype_logo005.rgb", tile_set);
    // process_image("images/skhype_logo006.rgb", tile_set);
    // process_image("images/skhype_logo007.rgb", tile_set);
    // process_image("images/skhype_logo008.rgb", tile_set);
    //process_image("images/skhype_logo015.rgb", tile_set);
    
    uint8 tiles_bitplaned[tile_set.size() * 8 * 8];
    uint8 cur_tile[8*8];
    uint8* tile_rows[8];
    for (int i = 0; i < tile_set.size(); i++)
    {
        for (int pixel_y = 0; pixel_y < 8; pixel_y++)
        {
            for (int pixel_x = 0; pixel_x < 8; pixel_x++)
            {
                // Convert 8-bit packed to quantized data (0 = palette #0, 1 = palette #255)
                if ((tile_set[i].pixels[pixel_y] & (1 << (7 - pixel_x))) != 0)
                {
                    cur_tile[pixel_y * 8 + pixel_x] = 255;
                }
                else
                {
                    cur_tile[pixel_y * 8 + pixel_x] = 0;
                }
            }
            // Collect tile rows into array
            tile_rows[pixel_y] = &cur_tile[pixel_y * 8];
        }
        // Bitplane the current tile into the temporary vram data
        bitplane_tile(tile_rows, &tiles_bitplaned[i*8*8]);
    }
    
    // Store the tiles starting at tile 65
    // Keep high tile map showing
    trans_vram_data(tiles_bitplaned, tile_set.size() * 8 * 8, (65 * 8 * 8) / 2, 4, 4);
    
    
    // Convert the images to a tile map
    uint8 tilemap[SSIZE_X*SSIZE_Y*2];
    
    tilize_image("images/skhype_logo000.rgb", tile_set, 65, tilemap);
    // Send the tile map for this image and switch to it
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 4, 0);
    
    // Delay between each animation frame
    for (int i = 0; i < 30; i++)
    {
        trans_nop();
    }
    
    // Prepare the next tile map
    tilize_image("images/skhype_logo001.rgb", tile_set, 65, tilemap);
    // Send the next tile map and switch to it
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, (32 * 8 * 8) / 2, 0, 4);
    
    for (int i = 0; i < 30; i++)
    {
        trans_nop();
    }
    
    tilize_image("images/skhype_logo002.rgb", tile_set, 65, tilemap);
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 4, 0);
    
    for (int i = 0; i < 30; i++)
    {
        trans_nop();
    }
    
    tilize_image("images/skhype_logo003.rgb", tile_set, 65, tilemap);
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, (32 * 8 * 8) / 2, 0, 4);
    
    for (int i = 0; i < 30; i++)
    {
        trans_nop();
    }
    
    tilize_image("images/skhype_logo004.rgb", tile_set, 65, tilemap);
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 4, 0);
    
    for (int i = 0; i < 30; i++)
    {
        trans_nop();
    }
    
    tilize_image("images/skhype_logo005.rgb", tile_set, 65, tilemap);
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, (32 * 8 * 8) / 2, 0, 4);
    
    return 0;
    
    cout << "Tile set size: " << tile_set.size() << endl;
    return 0;
    
    // Output the tile map into a 256x400 image
    for (int tile_y = 0; tile_y < 50; tile_y++)
    {
        for (int pixel_y = 0; pixel_y < 8; pixel_y++)
        {
            for (int tile_x = SSIZE_X - 1; tile_x >= 0; tile_x--)
            {
                for (int pixel_x = 0; pixel_x < 8; pixel_x++)
                { 
                    if (tile_y * SSIZE_X + tile_x >= tile_set.size())
                    {
                        cout.put(0);
                        cout.put(0);
                        cout.put(0);
                    }               
                    else
                    {
                        if ((tile_set[tile_y * SSIZE_X + tile_x].pixels[pixel_y] & (1 << (7 - pixel_x))) != 0)
                        {
                            cout.put(255);
                            cout.put(255);
                            cout.put(255);
                        }
                        else
                        {
                            cout.put(0);
                            cout.put(0);
                            cout.put(0);
                        }
                    }
                      
                }
            }
        }
    }

}