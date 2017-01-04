#include "data_sizes.h"

// Size in tiles
#define VSIZE_X 16
#define VSIZE_Y 14

#define VFRAME_RGB_SIZE (VSIZE_X * VSIZE_Y * 8 * 8 * 3)
#define VFRAME_SIZE (VSIZE_X * VSIZE_Y * 8 * 8)

// Number of iframes per frame, including one command header
#define SPEED 300
// Number of iframes of data per frame
#define DATA_IF (SPEED-1)
  
// Number of bytes per iframe
#define INFRAME_SIZE 8

typedef unsigned short uint16;

#define MASK(input, index, inbit, outbit, inflip) ((((input[index] ^ inflip) & (1 << inbit)) != 0) ? (1 << outbit) : 0)

void trans_one_iframe(uint8 *input, uint8 *output);
bool read_stdin(int length, uint8 * output);
bool trans_palette_stdin();
void trans_palette(uint8 *input, bool highmem);
bool trans_frame_stdin();
bool trans_frame(uint8 *input, bool highmem, bool final_highmem);
void trans_vram_data(uint8 *input, int length, int offset, uint8 intermediate_page, uint8 final_page);
void bitplane_tile(uint8 **rows, uint8 *output);
void trans_nop();