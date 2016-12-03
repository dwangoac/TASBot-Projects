#pragma once
int color_distance(unsigned char src_red, unsigned char src_green, unsigned char src_blue, unsigned char dest_red, unsigned char dest_green, unsigned char dest_blue);
void quantize_bitmap(unsigned char * input_data, unsigned char * output_data, int image_size_x, int image_size_y, unsigned char * palette_data, int palette_size);