#include <iostream>

#include "octree.h"
#include "data_sizes.h"


// Size in tiles
#define VSIZE_X 16
#define VSIZE_Y 14

// Number of iframes per frame, including one command header
#define SPEED 300
// Number of iframes of data per frame
#define DATA_IF (SPEED-1)
  
// Number of bytes per iframe
#define INFRAME_SIZE 8

typedef unsigned short uint16;

using namespace std;

#define MASK(input, index, inbit, outbit, inflip) ((((input[index] ^ inflip) & (1 << inbit)) != 0) ? (1 << outbit) : 0)

void bitplane_tile(uint8 **rows, uint8 *output);
void trans_vram_data(uint8 *input, int length, int offset, uint8 intermediate_page, uint8 final_page);
void trans_palette(uint8 *input, bool highmem);
bool trans_frame(uint8 *input, bool highmem, bool final_highmem);

void trans_one_iframe(uint8 *input, uint8 *output)
{
    uint8 INFLIP1 = 0x00;
    uint8 INFLIP2 = 0xAC;
    
    for (int i = 0; i < 16; i++)
    {
        output[i] = 0;
    }
    
    for (int i = 0; i < 8; i++)
    {
        output[0] |= MASK(input, 6-(i&4)/2, 0+2*(3-i%4), 7-i, INFLIP1);
        output[1] |= MASK(input, 2-(i&4)/2, 0+2*(3-i%4), 7-i, INFLIP1);
        output[2] |= MASK(input, 6-(i&4)/2, 1+2*(3-i%4), 7-i, INFLIP1);
        output[3] |= MASK(input, 2-(i&4)/2, 1+2*(3-i%4), 7-i, INFLIP1);
        output[8] |= MASK(input, 7-(i&4)/2, 0+2*(3-i%4), 7-i, INFLIP2);
        output[9] |= MASK(input, 3-(i&4)/2, 0+2*(3-i%4), 7-i, INFLIP2);
        output[10] |= MASK(input, 7-(i&4)/2, 1+2*(3-i%4), 7-i, INFLIP2);
        output[11] |= MASK(input, 3-(i&4)/2, 1+2*(3-i%4), 7-i, INFLIP2);
    }
}

bool read_stdin(int length, uint8 * output)
{
    char temp;
    for (int i = 0; i < length; i++)
    {
        if (!std::cin.get(temp))
        {
            return false;
        }
        output[i] = temp;
    }
}

bool trans_palette_stdin()
{
    uint8 input[768];
    
    read_stdin(768, input);
    
    trans_palette(input, false);
}

void trans_palette(uint8 *input, bool highmem)
{
    for (int i = 0; i < 256; i++)
    {
        uint8 r = (input[3*i+0] >> 3);
        uint8 g = (input[3*i+1] >> 3);
        uint8 b = (input[3*i+2] >> 3);
        uint16 c = 1024 * b + 32 * g + r;
        input[2*i+0] = c & 0xFF;
        input[2*i+1] = (c >> 8) & 0xFF;
    }

    uint8 output[SPEED * 16];
    for (int i = 0; i < SPEED*16; i++)
    {
        output[i] = 0;
    }

    for (int i = 0; i < 64; i++)
    {
        int o = (DATA_IF-1)-i;
        trans_one_iframe(&input[8*i], &output[16*o]);
    }
    
    //Load palette, load tilemap 0k, tiledata 0k.
    uint8 header[8] = {2, 0, 0, 0, 0, (highmem ? 0 : 4), 0, 0};
    trans_one_iframe(header, &output[(SPEED - 1) * 16]);
    
    for (int i = 0; i < SPEED*16; i++)
    {
        cout.put(output[i]);
    }
}

bool trans_frame_stdin()
{
    uint8 input[VSIZE_X*VSIZE_Y * 8*8];
    
    read_stdin(VSIZE_X*VSIZE_Y * 8*8, input);
    
    trans_frame(input, false, false);
}


bool trans_frame(uint8 *input, bool highmem, bool final_highmem)
{
    uint8 bitplaned_output[VSIZE_X*VSIZE_Y * 8*8];
        
    // Convert to bitplaned tiles
    uint8 *rows[8];
    for (int x = 0; x < VSIZE_X; x++)
    {
        for (int y = 0; y < VSIZE_Y; y++)
        {
            for (int row = 0; row < 8; row++)
            {
                rows[row] = &input[(y * 8 + row) * (VSIZE_X * 8) + (x * 8)];
            }
            bitplane_tile(rows, &bitplaned_output[(y*VSIZE_X + x) * 8*8]);
        }
    }
    
    trans_vram_data(bitplaned_output, VSIZE_X * VSIZE_Y * (8*8), (32768 - ((VSIZE_X * VSIZE_Y * (8*8)/2) * (highmem ? 1 : 2))) , (highmem ? 0 : 4), (final_highmem ? 4 : 0));
    return true;
}

// Length must be multiple of 8
void trans_vram_data(uint8 *input, int length, int offset, uint8 intermediate_page, uint8 final_page)
{
    // Step size per transfer in bytes
    int step_size = DATA_IF * 8;
    int transfers_needed = length / step_size + (length % step_size != 0 ? 1 : 0);
    
    uint8 output[16];
    for (int svframe = 0; svframe < transfers_needed; svframe++)
    {
        for (int iframe = 0; iframe < DATA_IF; iframe++)
        {
             // Only process new data if there is some available
            if ((svframe*step_size + ((DATA_IF - 1)-iframe)*8) < length)
            {
                trans_one_iframe(&input[svframe*step_size + ((DATA_IF - 1)-iframe)*8], output);
            }
            
            for (int i = 0; i < 16; i++)
            {
                cout.put(output[i]);
            }
        }
        
        int cur_offset = offset + svframe*step_size/2;
        int transfer_size = (svframe == (transfers_needed - 1) ? length - (step_size * (transfers_needed - 1)) : step_size);
        
        uint8 header[8] = {1, cur_offset & 0xFF, (cur_offset >> 8) & 0xFF, transfer_size & 0xFF, (transfer_size >> 8) & 0xFF, (svframe == (transfers_needed - 1) ? final_page : intermediate_page), 0, 0};
        trans_one_iframe(header, output);
        for (int i = 0; i < 16; i++)
        {
            cout.put(output[i]);
        }
    }

}

// Input: 8 rows of 8 bytes
// Output: 64 bytes, bitplaned for the SNES VRAM
void bitplane_tile(uint8 **rows, uint8 *output)
{
    for (int row = 0; row < 8; row++)
    {
        for (int bit = 0; bit < 8; bit++)
        {
            int output_index = (bit / 2) * 16 + (row * 2) + (bit % 2);
            output[output_index] = 0;
            for (int pixel = 0; pixel < 8; pixel++)
            {
                output[output_index] = (output[output_index] << 1) | ((rows[row][pixel] & (0x01 << bit)) >> bit);
            }
        }
    }
}

int main()
{
    uint8 video_frame[VSIZE_X * VSIZE_Y * 8 * 8 * 3];
    // Octree foo;
    // for (int i = 0; i < 120*104*3; i++)
    // {
        // char temp;
        // if (!std::cin.get(temp))
        // {
            // return 0;
        // }
        // video_frame[i] = temp;
        
        // if (i % 3 == 2)
        // {
            // foo.insert_color(video_frame[i-2], video_frame[i-1], video_frame[i]);
        // }
    // }
        
    // foo.reduce(256);
    
    // uint8 palette[256*3];
    // for (int i = 0; i < 256*3; i++)
    // {
        // palette[i] = 0;
    // }
    // foo.make_palette_table(palette);
    
    // /*
    // for (int i = 0; i < 256*3; i++)
    // {
        // cout.put(palette[i]);
    // }
    // */
    // trans_palette(palette, true);

    bool highmem = true;
    uint8 quan_data[VSIZE_X * VSIZE_Y * 8 * 8];
    char temp;
    while (true)
    {
        Octree foo;
        for (int i = 0; i < VSIZE_X * VSIZE_Y * 8 * 8 * 3; i++)
        {
            if (!std::cin.get(temp))
            {
                return 0;
            }
            video_frame[i] = temp;
            
            if (i % 3 == 2)
            {
                foo.insert_color(video_frame[i-2], video_frame[i-1], video_frame[i]);
            }
        }
        
        foo.reduce(256);
    
        uint8 palette[256*3];
        for (int i = 0; i < 256*3; i++)
        {
            palette[i] = 0;
        }
        foo.make_palette_table(palette);
        
        for (int i = 0; i < VSIZE_X * VSIZE_Y * 8 * 8; i++)
        {
            quan_data[i] = foo.find_color(video_frame[i*3+0],video_frame[i*3+1],video_frame[i*3+2]);
        }
        trans_frame(quan_data, highmem, !highmem);
        highmem = !highmem;
        trans_palette(palette, highmem);
        
        
        /*
        for (int i = 0; i < 120*112; i++)
        {
            cout.put(quan_data[i]);
        }
        return 0;
        */
        
    }

    return 0;
}