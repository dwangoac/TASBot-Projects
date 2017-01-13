#include <iostream>
#include <fstream>
#include <vector>
#include "../process_image_stream/process_image/data_sizes.h"
#include "../process_image_stream/process_image/trans.h"

// Screen area size
#define SSIZE_X 32
#define SSIZE_Y 28

#define set_tilemap_tile(tilemap, x, y, sizex, sizey, value) { \
  tilemap[y*sizex*2+x*2] = value & 0xFF; \
  tilemap[y*sizex*2+x*2+1] = (value >> 8) & 0xFF; \
}


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

uint8 * read_file(string filename)
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
          return 0;
      }
      
      image_file.seekg(0, ios::beg);
      image_file.read((char *)image_data, image_size);
      image_file.close();
    }
    else
    {
        cout << "Unable to open " << filename << endl;
        return 0;
    }
    return image_data;
}

void bitplane_pal_tile(uint8 * image_data, int tile_x, int tile_y, uint8 * bitplane_data)
{
    uint8 cur_tile[64];
    uint8* tile_rows[8];
    for (int pixel_y = 0; pixel_y < 8; pixel_y++)
    {
        for (int pixel_x = 0; pixel_x < 8; pixel_x++)
        {
            int image_offset = ((tile_y * 8) + pixel_y) * SSIZE_X * 8 * 3 + ((tile_x * 8) + pixel_x) * 3;
            cur_tile[pixel_y * 8 + pixel_x] = image_data[image_offset];
        }
        // Collect tile rows into array
        tile_rows[pixel_y] = &cur_tile[pixel_y * 8];
    }
    bitplane_tile(tile_rows, bitplane_data);
}

void replace_tile_with_color(uint8 * image_data, int tile_x, int tile_y, uint8 * tileset, uint8 * tilemap, int tile_offset, int * tile_counter)
{
    bitplane_pal_tile(image_data, tile_x, tile_y, &tileset[(*tile_counter)*8*8]);
    set_tilemap_tile(tilemap, tile_x, tile_y, SSIZE_X, SSIZE_Y, (tile_offset + (*tile_counter)));
    (*tile_counter)++;
}

int main ()
{
    trans_nop();
  
    // Palette: 0-254 = black, 255 = White
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
  
    // Send palette, switch to low tile map
    trans_palette(palette, true);    
  
    vector<tile_data> tile_set;
    // Process the border and the final result image to generate the required tiles
    process_image("images/border.rgb", tile_set);
    int border_tile_size = tile_set.size();
    process_image("images/portal.rgb", tile_set);
    int final_tile_size = tile_set.size();
    
    // Send the tile map for just the border to free up tiles
    uint8 tilemap[SSIZE_X*SSIZE_Y*2];
    tilize_image("images/border.rgb", tile_set, 65, tilemap);
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 0, 0);

    uint8 tiles_bitplaned[(final_tile_size - border_tile_size) * 8 * 8];
    
    // Bitplane the color#255 tiles
    uint8 cur_tile[8*8];
    uint8* tile_rows[8];
    for (int i = border_tile_size; i < final_tile_size; i++)
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
        bitplane_tile(tile_rows, &tiles_bitplaned[(i-border_tile_size)*8*8]);
    }
    
    // Send the new tiles
    trans_vram_data(tiles_bitplaned, (final_tile_size - border_tile_size) * 8 * 8, ((65 + border_tile_size) * 8 * 8) / 2, 0, 0);
    
    // Convert the border to tiles to use as a base
    tilize_image("images/border.rgb", tile_set, 65, tilemap);
    
    // Convert only the new data to tiles 
    int tile_counter = 0;
    uint8 new_tiles_bitplaned[200 * 8 * 8];
    uint8 * image_data = read_file("images/portal.rgb");
    // "PORTAL"
    replace_tile_with_color(image_data,  4, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  5, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  6, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data,  8, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  9, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 10, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 11, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 12, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 13, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 14, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 15, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 16, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 17, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 18, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 19, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 20, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 21, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 22, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 23, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 26, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 27, 1, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  4, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  5, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  6, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  8, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  9, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 10, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 11, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 12, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 13, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 14, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 15, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 16, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 17, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 18, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 19, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 20, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 21, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 22, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 23, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 26, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 27, 2, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  4, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  5, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  6, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  8, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  9, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 10, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 11, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 12, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 13, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 14, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 15, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 16, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 17, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 18, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 19, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 20, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 21, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 22, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 23, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 26, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 27, 3, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  4, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  5, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  6, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  8, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  9, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 10, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 11, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 12, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 13, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 14, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 15, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 16, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 17, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 18, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 19, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 20, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 21, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 22, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 23, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 26, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data, 27, 4, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  4, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  5, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data,  6, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
//    replace_tile_with_color(image_data,  7, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  8, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  9, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 10, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 11, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 12, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 13, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 14, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 15, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 16, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 17, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 18, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 19, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 20, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 21, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 22, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 23, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 26, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 27, 5, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);

    // Left half of person
    // replace_tile_with_color(image_data,  2,  9, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  3,  9, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  4,  9, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  5,  9, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  6,  9, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7,  9, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  2, 10, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  3, 10, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  4, 10, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  5, 10, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  6, 10, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 10, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  2, 11, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  3, 11, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  4, 11, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  5, 11, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  6, 11, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 11, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  2, 12, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  3, 12, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  4, 12, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  5, 12, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  6, 12, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 12, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  2, 13, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  3, 13, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  4, 13, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  5, 13, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  6, 13, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 13, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  2, 14, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  3, 14, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  4, 14, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  5, 14, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  6, 14, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 14, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  2, 15, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  3, 15, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  4, 15, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  5, 15, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  6, 15, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 15, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  2, 16, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  3, 16, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  4, 16, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  5, 16, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  6, 16, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 16, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  2, 17, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  3, 17, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  4, 17, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  5, 17, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  6, 17, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  7, 17, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  2, 18, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  3, 18, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  4, 18, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  5, 18, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  6, 18, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 18, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  2, 19, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  3, 19, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  4, 19, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  5, 19, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  6, 19, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 19, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  2, 20, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  3, 20, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  4, 20, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data,  5, 20, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  6, 20, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data,  7, 20, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    
    // Right half of person
    // replace_tile_with_color(image_data, 24,  8, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 25,  8, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 26,  8, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 27,  8, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 28,  8, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 29,  8, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24,  9, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25,  9, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 26,  9, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 27,  9, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 28,  9, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 29,  9, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 10, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 10, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 26, 10, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 27, 10, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 28, 10, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 29, 10, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 11, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 11, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 26, 11, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 27, 11, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 28, 11, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 29, 11, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 12, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 12, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 26, 12, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 27, 12, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 28, 12, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 29, 12, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 13, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 13, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 26, 13, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 27, 13, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 28, 13, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 29, 13, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 14, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 14, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 26, 14, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 27, 14, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 28, 14, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 29, 14, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 15, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 15, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 26, 15, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 27, 15, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 28, 15, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 29, 15, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 16, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 16, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 26, 16, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 27, 16, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 28, 16, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 29, 16, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 17, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 17, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 26, 17, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 27, 17, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 28, 17, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 29, 17, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 18, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 25, 18, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 26, 18, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 27, 18, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 28, 18, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 29, 18, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    replace_tile_with_color(image_data, 24, 19, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 25, 19, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 26, 19, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 27, 19, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 28, 19, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);
    // replace_tile_with_color(image_data, 29, 19, new_tiles_bitplaned, tilemap, 65+final_tile_size, &tile_counter);

    // Send color tiles
    trans_vram_data(new_tiles_bitplaned, 200 * 8 * 8, ((65 + final_tile_size) * 8 * 8) / 2, 0, 0);
    // Send new tile map
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, (32 * 8 * 8) / 2, 0, 0);

    // Fade in new stuff
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
        trans_palette(palette, false);  
    }

    // Set up tile map to have both borders and text as color #255
    tilize_image("images/portal.rgb", tile_set, 65, tilemap);
   
    // Fix the tile map to work with video again
    for (int tile_y = 0; tile_y < VSIZE_Y; tile_y++)
    {
        for (int tile_x = 0; tile_x < VSIZE_X; tile_x++)
        {
            //cout << tile_x << endl;
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
    trans_palette(palette, false);

    trans_nop();
    
    return 0;

        
    for (int j = 0; j < 1000; j++)
    {
        palette[255*3 + 0] = 255;
        palette[255*3 + 1] = 0;
        palette[255*3 + 2] = 0;
        trans_palette(palette, false);  
        trans_palette(palette, true);
        
        palette[255*3 + 0] = 0;
        palette[255*3 + 1] = 255;
        palette[255*3 + 2] = 0;
        trans_palette(palette, false);
        trans_palette(palette, true);  
    }
    trans_nop();
    
    return 0;

    trans_nop();
    
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