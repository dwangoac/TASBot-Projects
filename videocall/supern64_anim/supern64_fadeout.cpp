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
    trans_nop();
  
    // Palette: 0-253 = black, 254-255 = White
    uint8 palette[256*3];
    for (int i = 0; i < 256; i++)
    {
        palette[i*3 + 0] = 0;
        palette[i*3 + 1] = 0;
        palette[i*3 + 2] = 0;
    }
    palette[254*3 + 0] = 255;
    palette[254*3 + 1] = 255;
    palette[254*3 + 2] = 255;
    palette[255*3 + 0] = 255;
    palette[255*3 + 1] = 255;
    palette[255*3 + 2] = 255;

    // Send palette, switch to high tile map
    trans_palette(palette, false);
    
    // Re-generate the tileset for the border + final layout
    vector<tile_data> tile_set;
    process_image("images/border.rgb", tile_set);
    int border_tileset_size = tile_set.size();
    process_image("images/super_n64.rgb", tile_set);
    int final_tile_size = tile_set.size();
    
    // Generate a separate tile set for the text
    vector<tile_data> tile_set_text;
    process_image("images/text.rgb", tile_set_text);
    int text_tile_size = tile_set_text.size();

    // Convert the border to tiles
    uint8 tilemap[SSIZE_X*SSIZE_Y*2];
    tilize_image("images/border.rgb", tile_set, 65, tilemap);

    // Convert text-only to color tiles
    uint8 tilemap_text[SSIZE_X*SSIZE_Y*2];
    tilize_image("images/text.rgb", tile_set_text, 65+final_tile_size, tilemap_text);
    
    // Copy color text tiles to border tile map
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < 22; i++)
        {
            tilemap[(22+j)*SSIZE_X*2+(5+i)*2+0]=tilemap_text[(22+j)*SSIZE_X*2+(5+i)*2+0];
            tilemap[(22+j)*SSIZE_X*2+(5+i)*2+1]=tilemap_text[(22+j)*SSIZE_X*2+(5+i)*2+1];
        }
    }
    
    // Send tile map
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 4, 0);

    // Fade out text
    for (int j = 252; j >= 0; j-=4)
    {
        for (int i = 0; i < 256; i++)
        {
            palette[i*3 + 0] = 0;
            palette[i*3 + 1] = 0;
            palette[i*3 + 2] = 0;
        }
        palette[254*3 + 0] = j;
        palette[254*3 + 1] = j;
        palette[254*3 + 2] = j;
        palette[255*3 + 0] = 255;
        palette[255*3 + 1] = 255;
        palette[255*3 + 2] = 255;

        trans_palette(palette, true);  
    }

    trans_nop();
    
    return 0;

    

    
/*
    // Process the final result image first to generate the required tiles
    process_image("images/super_n64.rgb", tile_set);
    int final_tile_size = tile_set.size();
        
    // Generate a separate tile set for the text
    vector<tile_data> tile_set_text;
    process_image("images/text.rgb", tile_set_text);
    int text_tile_size = tile_set_text.size();
    
    uint8 tiles_bitplaned[(final_tile_size + text_tile_size) * 8 * 8];
    uint8 cur_tile[8*8];
    uint8* tile_rows[8];
    for (int i = 0; i < final_tile_size; i++)
    {
        for (int pixel_y = 0; pixel_y < 8; pixel_y++)
        {
            for (int pixel_x = 0; pixel_x < 8; pixel_x++)
            {
                if ((tile_set[i].pixels[pixel_y] & (1 << (7 - pixel_x))) != 0)
                {
                    cur_tile[pixel_y * 8 + pixel_x] = 255;
                }
                else
                {
                    cur_tile[pixel_y * 8 + pixel_x] = 0;
                }
            }
            tile_rows[pixel_y] = &cur_tile[pixel_y * 8];
        }
        bitplane_tile(tile_rows, &tiles_bitplaned[i*8*8]);
    }
    
    for (int i = 0; i < text_tile_size; i++)
    {
        for (int pixel_y = 0; pixel_y < 8; pixel_y++)
        {
            for (int pixel_x = 0; pixel_x < 8; pixel_x++)
            {
                if ((tile_set_text[i].pixels[pixel_y] & (1 << (7 - pixel_x))) != 0)
                {
                    cur_tile[pixel_y * 8 + pixel_x] = 254;
                }
                else
                {
                    cur_tile[pixel_y * 8 + pixel_x] = 0;
                }
            }
            tile_rows[pixel_y] = &cur_tile[pixel_y * 8];
        }
        bitplane_tile(tile_rows, &tiles_bitplaned[(final_tile_size+i)*8*8]);
    }
    
    // Store the tiles starting at tile 65
    // Keep high tile map showing
    trans_vram_data(tiles_bitplaned, (final_tile_size + text_tile_size) * 8 * 8, (65 * 8 * 8) / 2, 4, 4);
    
    // Convert the border to tiles
    uint8 tilemap[SSIZE_X*SSIZE_Y*2];
    tilize_image("images/border.rgb", tile_set, 65, tilemap);

    // Convert text to color tiles
    uint8 tilemap_text[SSIZE_X*SSIZE_Y*2];
    tilize_image("images/text.rgb", tile_set_text, 65+final_tile_size, tilemap_text);
    
    // Copy text tiles to border tile map
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < 22; i++)
        {
            tilemap[(22+j)*SSIZE_X*2+(5+i)*2+0]=tilemap_text[(22+j)*SSIZE_X*2+(5+i)*2+0];
            tilemap[(22+j)*SSIZE_X*2+(5+i)*2+1]=tilemap_text[(22+j)*SSIZE_X*2+(5+i)*2+1];
        }
    }
    
    // Send tile map
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 4, 0);
    
    // Fade in borders
    for (int j = 0; j < 256; j+=4)
    {
        for (int i = 0; i < 256; i++)
        {
            palette[i*3 + 0] = 0;
            palette[i*3 + 1] = 0;
            palette[i*3 + 2] = 0;
        }
        palette[255*3 + 0] = j;
        palette[255*3 + 1] = j;
        palette[255*3 + 2] = j;
        trans_palette(palette, true);  
    }
    
    // Wait
    for (int i = 0; i < 60; i++)
    {
        trans_nop();
    }
    
    // Fade in text
    for (int j = 0; j < 256; j+=4)
    {
        for (int i = 0; i < 256; i++)
        {
            palette[i*3 + 0] = 0;
            palette[i*3 + 1] = 0;
            palette[i*3 + 2] = 0;
        }
        palette[254*3 + 0] = j;
        palette[254*3 + 1] = j;
        palette[254*3 + 2] = j;
        palette[255*3 + 0] = 255;
        palette[255*3 + 1] = 255;
        palette[255*3 + 2] = 255;
        trans_palette(palette, true);  
    }
    
    // Set up tile map to have both borders and text as color #255
    tilize_image("images/super_n64.rgb", tile_set, 65, tilemap);

    // Fix the tile map to work with video again
    for (int tile_y = 0; tile_y < VSIZE_Y; tile_y++)
    {
        for (int tile_x = 0; tile_x < VSIZE_X; tile_x++)
        {
            int highmem_tile_addr = 576 + tile_y * VSIZE_X + tile_x;
            tilemap[((tile_y+7) * SSIZE_X * 2) + ((tile_x + 8) * 2) + 0] = highmem_tile_addr & 0xFF;
            tilemap[((tile_y+7) * SSIZE_X * 2) + ((tile_x + 8) * 2) + 1] = (highmem_tile_addr >> 8) & 0x0F;
        }
    }
    
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 4, 0);
    
    for (int tile_y = 0; tile_y < VSIZE_Y; tile_y++)
    {
        for (int tile_x = 0; tile_x < VSIZE_X; tile_x++)
        {
            int highmem_tile_addr = 800 + tile_y * VSIZE_X + tile_x;
            tilemap[((tile_y+7) * SSIZE_X * 2) + ((tile_x + 8) * 2) + 0] = highmem_tile_addr & 0xFF;
            tilemap[((tile_y+7) * SSIZE_X * 2) + ((tile_x + 8) * 2) + 1] = (highmem_tile_addr >> 8) & 0x0F;
        }
    }
    
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, (32 * 8 * 8) / 2, 4, 0);
    
    // Turn off color #254
    // Palette: 0-254 = black, 255 = white
    for (int i = 0; i < 256; i++)
    {
        palette[i*3 + 0] = 0;
        palette[i*3 + 1] = 0;
        palette[i*3 + 2] = 0;
    }
    palette[255*3 + 0] = 255;
    palette[255*3 + 1] = 255;
    palette[255*3 + 2] = 255;
    trans_palette(palette, true);  
    
    trans_nop();
    */
    return 0;
    
    cout << "Tile set size: " << tile_set.size() << endl;
    return 0;
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