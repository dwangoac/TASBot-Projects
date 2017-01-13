#include <iostream>
#include <fstream>
#include <vector>
#include "../process_image_stream/process_image/data_sizes.h"
#include "../process_image_stream/process_image/trans.h"

// Screen area size
#define SSIZE_X 32
#define SSIZE_Y 28

#define set_pixel(image_data, x, y, sizex, sizey, value) image_data[y*sizex*3+x*3] = value

#define set_tilemap_tile(tilemap, x, y, sizex, sizey, value) { \
  tilemap[y*sizex*2+x*2] = value & 0xFF; \
  tilemap[y*sizex*2+x*2+1] = (value >> 8) & 0xFF; \
}

using namespace std;

typedef struct _tile_data
{
    uint8 pixels[8];
} tile_data;

typedef struct _tile_data_pal
{
    uint8 pixels[64];
} tile_data_pal;


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

bool process_image(string filename, vector<tile_data> &tile_set)
{
    uint8 * image_data = read_file(filename);
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
    uint8 * image_data = read_file(filename);

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

int main ()
{
    trans_nop();
  
    uint8 palette[256*3];
    // Fade out old stuff
    for (int j = 252; j >= 0; j-=4)
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

        trans_palette(palette, false);  
    }

    // Palette: 0-255 = black
    for (int i = 0; i < 256; i++)
    {
        palette[i*3 + 0] = 0;
        palette[i*3 + 1] = 0;
        palette[i*3 + 2] = 0;
    }
  
    // Send palette, switch to high tile map
    trans_palette(palette, false);
  
    // Convert images to tiles
    vector<tile_data> tile_set;
    // Process images backwards so that tiles used in later images are first in the tile set
    // Start with the final frame so that it is always in the tile set
    process_image("images/skhype_logo015.rgb", tile_set);
    process_image("images/skhype_logo005.rgb", tile_set);

    process_image("images/skhype_logo003.rgb", tile_set);
    process_image("images/skhype_logo002.rgb", tile_set);
    process_image("images/skhype_logo001.rgb", tile_set);
    process_image("images/skhype_logo000.rgb", tile_set);
    
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
    
    // Fade in "Hype!"
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
  
    // Send palette, switch to low tile map
    trans_palette(palette, true);

    
    
    // Delay between each animation frame

    for (int i = 0; i < 5*60-10; i++)
    {
        trans_nop();
    }
    
    for (int j = 0; j < 10; j++)
    {
        for (int i = 0; i < 256; i++)
        {
            palette[i*3 + 0] = 0;
            palette[i*3 + 1] = 0;
            palette[i*3 + 2] = 0;
        }
        palette[255*3 + 0] = 255 - ((255-0x7a)/40 * (j+1));
        palette[255*3 + 1] = 255 - ((255-0xd5)/40 * (j+1));
        palette[255*3 + 2] = 255 - ((255-0xff)/40 * (j+1));
        
        trans_palette(palette, true);
    }
    
    // Prepare the next tile map
    tilize_image("images/skhype_logo001.rgb", tile_set, 65, tilemap);
    // Send the next tile map and switch to it
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, (32 * 8 * 8) / 2, 0, 4);
    
    /*
    for (int i = 0; i < 10; i++)
    {
        trans_nop();
    }
    */
    
    for (int j = 10; j < 20; j++)
    {
        for (int i = 0; i < 256; i++)
        {
            palette[i*3 + 0] = 0;
            palette[i*3 + 1] = 0;
            palette[i*3 + 2] = 0;
        }
        palette[255*3 + 0] = 255 - ((255-0x7a)/40 * (j+1));
        palette[255*3 + 1] = 255 - ((255-0xd5)/40 * (j+1));
        palette[255*3 + 2] = 255 - ((255-0xff)/40 * (j+1));
        
        trans_palette(palette, false);
    }
    
    
    tilize_image("images/skhype_logo002.rgb", tile_set, 65, tilemap);
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 4, 0);
   
/*   
    for (int i = 0; i < 10; i++)
    {
        trans_nop();
    }
*/    

    for (int j = 20; j < 30; j++)
    {
        for (int i = 0; i < 256; i++)
        {
            palette[i*3 + 0] = 0;
            palette[i*3 + 1] = 0;
            palette[i*3 + 2] = 0;
        }
        palette[255*3 + 0] = 255 - ((255-0x7a)/40 * (j+1));
        palette[255*3 + 1] = 255 - ((255-0xd5)/40 * (j+1));
        palette[255*3 + 2] = 255 - ((255-0xff)/40 * (j+1));
        
        trans_palette(palette, true);
    }

    tilize_image("images/skhype_logo003.rgb", tile_set, 65, tilemap);
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, (32 * 8 * 8) / 2, 0, 4);
    
    /*
    for (int i = 0; i < 10; i++)
    {
        trans_nop();
    }
    */
    
    for (int j = 30; j < 40; j++)
    {
        // Palette: 0-254 = black, 255 = white
        for (int i = 0; i < 256; i++)
        {
            palette[i*3 + 0] = 0;
            palette[i*3 + 1] = 0;
            palette[i*3 + 2] = 0;
        }
        palette[255*3 + 0] = 255 - ((255-0x7a)/40 * (j+1));
        palette[255*3 + 1] = 255 - ((255-0xd5)/40 * (j+1));
        palette[255*3 + 2] = 255 - ((255-0xff)/40 * (j+1));
        
        trans_palette(palette, false);
    }
    
    tilize_image("images/skhype_logo004.rgb", tile_set, 65, tilemap);
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 4, 0);
    
    for (int i = 0; i < 90; i++)
    {
        trans_nop();
    }
    
    tilize_image("images/skhype_logo005.rgb", tile_set, 65, tilemap);
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, (32 * 8 * 8) / 2, 0, 4);
    
    for (int i = 0; i < 6*60-14-90; i++)
    {
        trans_nop();
    }
    
    // Reprocess the tile set to include the final frame, the current frame, and the next frames
    tile_set.clear();
    process_image("images/skhype_logo015.rgb", tile_set);
    process_image("images/skhype_logo005.rgb", tile_set);

    process_image("images/skhype_logo008a.rgb", tile_set);
    
    int line_tiles_start = tile_set.size();
    process_image("images/skhype_logo007.rgb", tile_set);
    process_image("images/skhype_logo006.rgb", tile_set);
    
    // Bitplane the new tiles
    uint8 tiles_bitplaned2[tile_set.size() * 8 * 8];
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
        bitplane_tile(tile_rows, &tiles_bitplaned2[i*8*8]);
    }

    // Store the new tiles
    // Keep high tile map showing
    trans_vram_data(tiles_bitplaned2, tile_set.size() * 8 * 8, (65 * 8 * 8) / 2, 4, 4);
    
    // prepare the next tile map using the new numbers
    tilize_image("images/skhype_logo006.rgb", tile_set, 65, tilemap);
    // Send the next tile map and switch to it
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 4, 0);

    /*
    for (int i = 0; i < 10; i++)
    {
        trans_nop();
    }*/
    
    for (int j = 0; j < 10; j++)
    {
        for (int i = 0; i < 256; i++)
        {
            palette[i*3 + 0] = 0;
            palette[i*3 + 1] = 0;
            palette[i*3 + 2] = 0;
        }
        palette[255*3 + 0] = 0x7a + ((255-0x7a)/30 * (j+1));
        palette[255*3 + 1] = 0xd5 + ((255-0xd5)/30 * (j+1));
        palette[255*3 + 2] = 0xff + ((255-0xff)/30 * (j+1));
        
        trans_palette(palette, true);
    }
    
    tilize_image("images/skhype_logo007.rgb", tile_set, 65, tilemap);
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, (32 * 8 * 8) / 2, 0, 4);
    
    /*
    for (int i = 0; i < 10; i++)
    {
        trans_nop();
    }
    */
    
    for (int j = 10; j < 20; j++)
    {
        for (int i = 0; i < 256; i++)
        {
            palette[i*3 + 0] = 0;
            palette[i*3 + 1] = 0;
            palette[i*3 + 2] = 0;
        }
        palette[255*3 + 0] = 0x7a + ((255-0x7a)/30 * (j+1));
        palette[255*3 + 1] = 0xd5 + ((255-0xd5)/30 * (j+1));
        palette[255*3 + 2] = 0xff + ((255-0xff)/30 * (j+1));
        
        trans_palette(palette, false);
    }
    
    tilize_image("images/skhype_logo008a.rgb", tile_set, 65, tilemap);
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 4, 0);
    
    /*
    for (int i = 0; i < 10; i++)
    {
        trans_nop();
    }
    */
    
    for (int j = 20; j < 30; j++)
    {
        for (int i = 0; i < 256; i++)
        {
            palette[i*3 + 0] = 0;
            palette[i*3 + 1] = 0;
            palette[i*3 + 2] = 0;
        }
        palette[255*3 + 0] = 0x7a + ((255-0x7a)/30 * (j+1));
        palette[255*3 + 1] = 0xd5 + ((255-0xd5)/30 * (j+1));
        palette[255*3 + 2] = 0xff + ((255-0xff)/30 * (j+1));
        
        trans_palette(palette, true);
    }
    
    trans_nop();
    
    // 8b is the starting point for the lines animation
    uint8 * image_data = read_file("images/skhype_logo008b.rgb");
    
    // Add pixels of a specific color to the image data at certain positions
    int color_counter = 1;
    for (int i = 161; i < 201; i++)
    {
        set_pixel(image_data, i, 198, SSIZE_X*8, SSIZE_Y*8, color_counter);
        if (i > 162)
        {
            set_pixel(image_data, (128-(i-128)-1), 198, SSIZE_X*8, SSIZE_Y*8, color_counter);
        }
        
        color_counter++;
    }
    
    // Skip a pixel
    color_counter++;
    
    set_pixel(image_data, 202, 198, SSIZE_X*8, SSIZE_Y*8, color_counter);
    set_pixel(image_data, (128-(202-128)-1), 198, SSIZE_X*8, SSIZE_Y*8, color_counter);
    color_counter++;
    
    color_counter++;
        
    for (int i = 0; i < 4; i++)
    {
        set_pixel(image_data, (204+i), (198-i), SSIZE_X*8, SSIZE_Y*8, color_counter);
        set_pixel(image_data, (128-((204+i)-128)-1), (198-i), SSIZE_X*8, SSIZE_Y*8, color_counter);
        
        color_counter++;
    }
    
    color_counter++;
    
    set_pixel(image_data, 207, 193, SSIZE_X*8, SSIZE_Y*8, color_counter);
    set_pixel(image_data, (128-(207-128)-1), 193, SSIZE_X*8, SSIZE_Y*8, color_counter);
    
    color_counter++;
    
    for (int i = 0; i <= 191-42; i++)
    {
        set_pixel(image_data, 207, (191-i), SSIZE_X*8, SSIZE_Y*8, color_counter);
        set_pixel(image_data, (128-(207-128)-1), (191-i), SSIZE_X*8, SSIZE_Y*8, color_counter);
        
        if (i >= 15 && i <= 39)
        {
            set_pixel(image_data, (208+(i-15)), 177, SSIZE_X*8, SSIZE_Y*8, color_counter);
            set_pixel(image_data, (128-((208+(i-15))-128)-1), 177, SSIZE_X*8, SSIZE_Y*8, color_counter);
        }
        
        if (i >= 18 && i <= 65)
        {
            set_pixel(image_data, (208+(i-18)), 174, SSIZE_X*8, SSIZE_Y*8, color_counter);
            set_pixel(image_data, (128-((208+(i-18))-128)-1), 174, SSIZE_X*8, SSIZE_Y*8, color_counter);
        }
        
        color_counter++;
    }
    
    color_counter++;
    
    set_pixel(image_data, 207, 40, SSIZE_X*8, SSIZE_Y*8, color_counter);
    set_pixel(image_data, (128-(207-128)-1), 40, SSIZE_X*8, SSIZE_Y*8, color_counter);
    color_counter++;
    
    color_counter++;
    
    for (int i = 0; i < 4; i++)
    {
        set_pixel(image_data, (207-i), (38-i), SSIZE_X*8, SSIZE_Y*8, color_counter);
        set_pixel(image_data, (128-((207-i)-128)-1), (38-i), SSIZE_X*8, SSIZE_Y*8, color_counter);
        
        color_counter++;
    }
    
    color_counter++;
    
    set_pixel(image_data, 202, 35, SSIZE_X*8, SSIZE_Y*8, color_counter);
    set_pixel(image_data, (128-(202-128)-1), 35, SSIZE_X*8, SSIZE_Y*8, color_counter);
    color_counter++;

    color_counter++;

    for (int i = 0; i <= 28; i++)
    {
        set_pixel(image_data, (200-i), 35, SSIZE_X*8, SSIZE_Y*8, color_counter);
        set_pixel(image_data, (128-((200-i)-128)-1), 35, SSIZE_X*8, SSIZE_Y*8, color_counter);
        
        color_counter++;
    }
    
    /*
    for (int i = 0; i < SSIZE_X*SSIZE_Y*8*8*3; i++)
    {
        cout.put(image_data[i]);
    }
    */
    
    
    // Finished adding pixels
    // Convert to tiles and put in vram
    
    
    int tile_counter = 0;
    uint8 line_tiles_bitplaned[133 * 8 * 8];
    for (int i = 0; i < 5; i++)
    {
        bitplane_pal_tile(image_data, (6+i), 4, &line_tiles_bitplaned[tile_counter*8*8]);
        set_tilemap_tile(tilemap, (6+i), 4, SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
        tile_counter++;
        
        bitplane_pal_tile(image_data, (21+i), 4, &line_tiles_bitplaned[tile_counter*8*8]);
        set_tilemap_tile(tilemap, (21+i), 4, SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
        tile_counter++;
    }
    
    for (int i = 0; i < 20; i++)
    {
        bitplane_pal_tile(image_data, 6, (5+i), &line_tiles_bitplaned[tile_counter*8*8]);
        set_tilemap_tile(tilemap, 6, (5+i), SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
        tile_counter++;
        
        bitplane_pal_tile(image_data, 25, (5+i), &line_tiles_bitplaned[tile_counter*8*8]);
        set_tilemap_tile(tilemap, 25, (5+i), SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
        tile_counter++;
    }
    
    for (int i = 0; i < 6; i++)
    {
        bitplane_pal_tile(image_data, i, 21, &line_tiles_bitplaned[tile_counter*8*8]);
        set_tilemap_tile(tilemap, i, 21, SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
        tile_counter++;
        
        bitplane_pal_tile(image_data, (26+i), 21, &line_tiles_bitplaned[tile_counter*8*8]);
        set_tilemap_tile(tilemap, (26+i), 21, SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
        tile_counter++;
    }
    
    for (int i = 0; i < 4; i++)
    {
        bitplane_pal_tile(image_data, (2+i), 22, &line_tiles_bitplaned[tile_counter*8*8]);
        set_tilemap_tile(tilemap, (2+i), 22, SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
        tile_counter++;
        
        bitplane_pal_tile(image_data, (26+i), 22, &line_tiles_bitplaned[tile_counter*8*8]);
        set_tilemap_tile(tilemap, (26+i), 22, SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
        tile_counter++;
    }
    
    for (int i = 0; i < 5; i++)
    {
        bitplane_pal_tile(image_data, (7+i), 24, &line_tiles_bitplaned[tile_counter*8*8]);
        set_tilemap_tile(tilemap, (7+i), 24, SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
        tile_counter++;
        
        bitplane_pal_tile(image_data, (20+i), 24, &line_tiles_bitplaned[tile_counter*8*8]);
        set_tilemap_tile(tilemap, (20+i), 24, SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
        tile_counter++;
    }
    
    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < 12; i++)
        {
            bitplane_pal_tile(image_data, (10+i), (21+j), &line_tiles_bitplaned[tile_counter*8*8]);
            set_tilemap_tile(tilemap, (10+i), (21+j), SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
            tile_counter++;
        }
    }
    
    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < 3; i++)
        {
            bitplane_pal_tile(image_data, (29+i), (25+j), &line_tiles_bitplaned[tile_counter*8*8]);
            set_tilemap_tile(tilemap, (29+i), (25+j), SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
            tile_counter++;
        }
    }
    
    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            bitplane_pal_tile(image_data, (28+i), j, &line_tiles_bitplaned[tile_counter*8*8]);
            set_tilemap_tile(tilemap, (28+i), j, SSIZE_X, SSIZE_Y, (65+line_tiles_start+tile_counter));
            tile_counter++;
        }
    }
    
    // Set up palette to show the lines at the bottom of the screen
    for (int i = 0; i < 256; i++)
    {
        palette[i*3 + 0] = 0;
        palette[i*3 + 1] = 0;
        palette[i*3 + 2] = 0;
    }
    for (int i = 1; i <= 0x1B; i++)
    {
        palette[i*3 + 0] = 255;
        palette[i*3 + 1] = 255;
        palette[i*3 + 2] = 255;
    }
    palette[0] = 0;
    palette[1] = 0;
    palette[2] = 0;
    palette[255*3 + 0] = 255;
    palette[255*3 + 1] = 255;
    palette[255*3 + 2] = 255;

    trans_palette(palette, true);    
    
    // Send color tiles
    trans_vram_data(line_tiles_bitplaned, 133 * 8 * 8, ((65 + line_tiles_start) * 8 * 8) / 2, 0, 0);
    // Send new tile map
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, (32 * 8 * 8) / 2, 0, 4);
    
  
    // "Draw" lines by changing the colors in the palette
    for (int j = 0; j < 128; j++)
    {
        for (int i = 0; i < 256; i++)
        {
            palette[i*3 + 0] = 0;
            palette[i*3 + 1] = 0;
            palette[i*3 + 2] = 0;
        }
        for (int i = 0x0C; i < j*2; i++)
        {
            palette[i*3 + 0] = 255;
            palette[i*3 + 1] = 255;
            palette[i*3 + 2] = 255;
        }
        if (j < 0x1B)
        {
            for (int i = j; i <= j+0x1B; i++)
            {
                palette[i*3 + 0] = 255;
                palette[i*3 + 1] = 255;
                palette[i*3 + 2] = 255;
            }
        }

        
        palette[0] = 0;
        palette[1] = 0;
        palette[2] = 0;
        palette[255*3 + 0] = 255;
        palette[255*3 + 1] = 255;
        palette[255*3 + 2] = 255;

        
        trans_palette(palette, false);    
    }
    
    // Fade in icons
    for (int j = 0; j < 256; j+=4)
    {
        for (int i = 0; i < 256; i++)
        {
            palette[i*3 + 0] = 0;
            palette[i*3 + 1] = 0;
            palette[i*3 + 2] = 0;
        }
        for (int i = 0x0c; i < 256; i++)
        {
            palette[i*3 + 0] = 255;
            palette[i*3 + 1] = 255;
            palette[i*3 + 2] = 255;
        }
        
        palette[0] = 0;
        palette[1] = 0;
        palette[2] = 0;
        palette[254*3 + 0] = j;
        palette[254*3 + 1] = j;
        palette[254*3 + 2] = j;
        
        palette[255*3 + 0] = 255;
        palette[255*3 + 1] = 255;
        palette[255*3 + 2] = 255;

        
        trans_palette(palette, false);  
    }
    
    // Submit the final frame
    tilize_image("images/skhype_logo015.rgb", tile_set, 65, tilemap);
    // Write the tile map to high and low, end on low
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 4, 0);
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, (32 * 8 * 8) / 2, 0, 0);

    // Change palette to all black except 255
    for (int i = 0; i < 256; i++)
    {
        palette[i*3 + 0] = 0;
        palette[i*3 + 1] = 0;
        palette[i*3 + 2] = 0;
    }
    palette[0] = 0;
    palette[1] = 0;
    palette[2] = 0;
    palette[255*3 + 0] = 255;
    palette[255*3 + 1] = 255;
    palette[255*3 + 2] = 255;
    trans_palette(palette, true);
    
    

    // Fill video area tiles with black
    uint8 blank_video_area[VSIZE_X * VSIZE_Y * 8 * 8];
    for (int i = 0; i < VSIZE_X * VSIZE_Y * 8 * 8; i++)
    {
        blank_video_area[i] = 0;
    }
    trans_frame(blank_video_area, false, true);
    trans_frame(blank_video_area, true, true);

    
    // Re-write video tiles to tilemaps and upload them, end on low
    for (int tile_y = 0; tile_y < VSIZE_Y; tile_y++)
    {
        for (int tile_x = 0; tile_x < VSIZE_X; tile_x++)
        {
            int highmem_tile_addr = 576 + tile_y * VSIZE_X + tile_x;
            tilemap[((tile_y+7) * SSIZE_X * 2) + ((tile_x + 8) * 2) + 0] = highmem_tile_addr & 0xFF;
            tilemap[((tile_y+7) * SSIZE_X * 2) + ((tile_x + 8) * 2) + 1] = (highmem_tile_addr >> 8) & 0x0F;
        }
    }
    
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, 0, 0, 0);
    for (int tile_y = 0; tile_y < VSIZE_Y; tile_y++)
    {
        for (int tile_x = 0; tile_x < VSIZE_X; tile_x++)
        {
            int highmem_tile_addr = 800 + tile_y * VSIZE_X + tile_x;
            tilemap[((tile_y+7) * SSIZE_X * 2) + ((tile_x + 8) * 2) + 0] = highmem_tile_addr & 0xFF;
            tilemap[((tile_y+7) * SSIZE_X * 2) + ((tile_x + 8) * 2) + 1] = (highmem_tile_addr >> 8) & 0x0F;
        }
    }
    
    trans_vram_data(tilemap, SSIZE_X*SSIZE_Y*2, (32 * 8 * 8) / 2, 0, 0);



    trans_nop();
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