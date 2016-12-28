#include <iostream>

// Size in tiles
#define VSIZE_X 15
#define VSIZE_Y 13

// Number of iframes per frame, including one command header
#define SPEED 261
// Number of iframes of data per frame
#define DATA_IF (SPEED-1)
  
// Number of bytes per iframe
#define INFRAME_SIZE 8

using namespace std;

typedef unsigned char uint8;
typedef unsigned short uint16;

#define MASK(input, index, inbit, outbit, inflip) ((((input[index] ^ inflip) & (1 << inbit)) != 0) ? (1 << outbit) : 0)

void bitplane_tile(uint8 **rows, uint8 *output);
void trans_vram_data(uint8 *input, int length, int offset, uint8 intermediate_page, uint8 final_page);

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

void trans_palette()
{
    uint8 input[768];
    uint8 output[4800];
    char temp;
    
    for (int i = 0; i < 768; i++)
    {
        if (!std::cin.get(temp))
        {
            return;
        }
        input[i] = temp;
    }
    
    for (int i = 0; i < 256; i++)
    {
        uint8 r = (input[3*i+0] >> 3);
        uint8 g = (input[3*i+1] >> 3);
        uint8 b = (input[3*i+2] >> 3);
        uint16 c = 1024 * b + 32 * g + r;
        input[2*i+0] = c & 0xFF;
        input[2*i+1] = (c >> 8) & 0xFF;
    }
    
    for (int i = 0; i < 16*300; i++)
    {
        output[i] = 0;
    }

    for (int i = 0; i < 64; i++)
    {
        int o = (300-2)-i;
        trans_one_iframe(&input[8*i], &output[16*o]);
    }
    
    //Load palette, load tilemap 0k, tiledata 0k.
    uint8 header[8] = {2, 0, 0, 0, 0, 0, 0, 0};
    trans_one_iframe(header, &output[4784]);
    
    for (int i = 0; i < 16*300; i++)
    {
        cout.put(output[i]);
    }
}

bool trans_frame(bool highmem)
{
    char temp;
    
    uint8 input[VSIZE_X*VSIZE_Y * 8*8];
    uint8 bitplaned_output[VSIZE_X*VSIZE_Y * 8*8];
    
    // Read in palettized pixel data
    for (int i = 0; i < VSIZE_X*VSIZE_Y * 8*8; i++)
    {
        if (!std::cin.get(temp))
        {
            return false;
        }
        input[i] = temp;
    }
    
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
    
    trans_vram_data(bitplaned_output, VSIZE_X * VSIZE_Y * (8*8), (32768 - ((VSIZE_X * VSIZE_Y * (8*8)/2) * (highmem ? 1 : 2))) , (highmem ? 0 : 4), (highmem ? 4 : 0));
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
    //trans_palette();
    bool highmem = true;
    while (true)
    {
        if (!trans_frame(highmem))
        {  
            return 0;
        }
        highmem = !highmem;
    }
    return 0;
}