#include <iostream>
#include <fstream>
#include <chrono>
#include "quantize.h"
using namespace std;

int get_bitmap_data_offset(unsigned char * bmp_data)
{
	return (bmp_data[0x0d] << 24) | (bmp_data[0x0c] << 16) | (bmp_data[0x0b] << 8) | bmp_data[0x0a];;
}

// Takes an 8-bit truecolor (no color map) BMP image, converts it to an array of pixels in R,G,B order
void read_bmp(unsigned char * bmp_data, unsigned char * bitmap_data, int width, int height)
{
	// Get the pointer to the bitmap data
	int bitmap_data_offset = get_bitmap_data_offset(bmp_data);

	// read image data into new bitmap array
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int bmp_index = bitmap_data_offset + ((height - 1) - y)*(width * 3) + x * 3;
			int output_index = y*(width * 3) + (x * 3);
			bitmap_data[output_index + 0] = bmp_data[bmp_index + 2];
			bitmap_data[output_index + 1] = bmp_data[bmp_index + 1];
			bitmap_data[output_index + 2] = bmp_data[bmp_index + 0];
		}
	}
}

int main(int argc, char *argv[])
{
	using std::chrono::duration_cast;
	using std::chrono::microseconds;
	typedef std::chrono::high_resolution_clock clock;


	if (argc != 4)
	{
		cout << "usage: " << argv[0] << " <input image> <palette> <output file>\n";
		return -1;
	}

	unsigned char * palette_data;

	ifstream palette_file(argv[2], ios::in | ios::binary | ios::ate);
	if (palette_file.is_open())
	{
		streampos palette_size;
		palette_size = palette_file.tellg();
		palette_data = new unsigned char[palette_size];
		palette_file.seekg(0, ios::beg);
		palette_file.read((char *)palette_data, palette_size);
		palette_file.close();
	}
	else
	{
		cout << "Unable to open palette";
		return -1;
	}
	
	// Convert palette to 15 bit
	for (int i = 0; i < 255; i++)
	{
		palette_data[i * 3 + 0] = palette_data[i * 3 + 0] & 0xF8;
		palette_data[i * 3 + 1] = palette_data[i * 3 + 1] & 0xF8;
		palette_data[i * 3 + 2] = palette_data[i * 3 + 2] & 0xF8;
	}
	
	
	char * input_image_data;

	ifstream image_file(argv[1], ios::in | ios::binary | ios::ate);
	streampos image_size;
	if (image_file.is_open())
	{
		image_size = image_file.tellg();
		input_image_data = new char[image_size];
		image_file.seekg(0, ios::beg);
		image_file.read(input_image_data, image_size);
		image_file.close();
	}
	else
	{
		cout << "Unable to open image file";
	}

	


	
	unsigned char * large_bitmap = new unsigned char[896 * 832 * 3];
	read_bmp((unsigned char*)input_image_data, large_bitmap, 896, 832);
	
	int image_width = 112;
	int image_height = 104;
	
	unsigned char * input_bitmap = new unsigned char[112 * 104 * 3];
	
	for (int tile_y = 0; tile_y < image_height; tile_y++)
	{
		for (int tile_x = 0; tile_x < image_width; tile_x++)
		{
			unsigned int r_total = 0;
			unsigned int g_total = 0;
			unsigned int b_total = 0;
			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					int index = ((tile_y * 8) + y) * (image_width * 8 * 3) + ((tile_x * 8) + x) * 3;
					r_total += large_bitmap[index + 0];
					g_total += large_bitmap[index + 1];
					b_total += large_bitmap[index + 2];
				}
			}

			int index = tile_y * (image_width * 3) + tile_x * 3;
			// Downshift by 6 (divide by (8*8)=64)
			input_bitmap[index + 0] = r_total >> 6;
			input_bitmap[index + 1] = g_total >> 6;
			input_bitmap[index + 2] = b_total >> 6;
		}
	}
	

	/*
	unsigned char ** quantized_data = new unsigned char*[8];
	int best_fit_distances[19 * 17];
	unsigned char tile_best_fit[19*17];
	for (int i = 0; i < 8; i++)
	{
		unsigned char* cur_palette = (unsigned char *)&palette_data[i*(3 * 15)];

		quantized_data[i] = new unsigned char[152 * 136];
		unsigned char* cur_quantized_data = quantized_data[i];

		// Quantize each image to each palette
		quantize_bitmap((unsigned char *)input_bitmap, quantized_data[i], 152, 136, cur_palette, 15);

		
		// Go tile by tile
		for (int tile_y = 0; tile_y < 17; tile_y++)
		{
			for (int tile_x = 0; tile_x < 19; tile_x++)
			{
				// Calculate distance of tile from input image
				int cur_distance = 0;
				for (int y = 0; y < 8; y++)
				{
					for (int x = 0; x < 8; x++)
					{
						int quantized_index = ((tile_y * 8) + y) * 152 + ((tile_x * 8) + x);
						int color_index = cur_quantized_data[quantized_index];

						int src_red = cur_palette[color_index * 3];
						int src_green = cur_palette[color_index * 3 + 1];
						int src_blue = cur_palette[color_index * 3 + 2];

						int pixel_index = ((tile_y * 8) + y) * (152 * 3) + ((tile_x * 8) + x) * 3;
						cur_distance += color_distance(src_red, src_green, src_blue, input_bitmap[pixel_index + 0], input_bitmap[pixel_index + 1], input_bitmap[pixel_index + 2]);
					}
				}
				if (i == 0 || cur_distance < best_fit_distances[tile_y * 19 + tile_x])
				{
					best_fit_distances[tile_y * 19 + tile_x] = cur_distance;
					tile_best_fit[tile_y * 19 + tile_x] = i;
				}
			}
		}
	}

	// We now have a list of best-fit palettes for each tile, and and each tile indexed into the palette

	// Output a bmp
	char * output_filedata = new char[image_size];
	for (int i = 0; i < image_size; i++)
	{
		output_filedata[i] = input_image_data[i];
	}

	for (int tile_y = 0; tile_y < 17; tile_y++)
	{
		for (int tile_x = 0; tile_x < 19; tile_x++)
		{
			unsigned char* cur_palette = (unsigned char *)&palette_data[tile_best_fit[tile_y * 19 + tile_x] * (3 * 15)];
			unsigned char* cur_quantized_data = quantized_data[tile_best_fit[tile_y * 19 + tile_x]];
			for (int y = 0; y < 8; y++)
			{
				for (int x = 0; x < 8; x++)
				{
					int quantized_index = ((tile_y * 8) + y) * 152 + ((tile_x * 8) + x);
					int color_index = cur_quantized_data[quantized_index];

					int src_red = cur_palette[color_index * 3];
					int src_green = cur_palette[color_index * 3 + 1];
					int src_blue = cur_palette[color_index * 3 + 2];

					int output_index = 0x36 + (135 - ((tile_y * 8) + y)) * (152 * 3) + ((tile_x * 8) + x) * 3;
					output_filedata[output_index + 2] = src_blue;
					output_filedata[output_index + 1] = src_green;
					output_filedata[output_index + 0] = src_red;
				}
			}
		}
	}
	ofstream outbin(argv[3], ios::binary);
	outbin.write(reinterpret_cast <const char*> (output_filedata), image_size);
	outbin.close();
	*/



	
	
	unsigned char * quantized_data = new unsigned char[image_width * image_height];
	auto start = clock::now();
	quantize_bitmap(input_bitmap, quantized_data, image_width, image_height, palette_data, 255);
	auto end = clock::now();
	std::cout << duration_cast<microseconds>(end - start).count() << "us\n";

	char * output_filedata = new char[image_size];
	for (int i = 0; i < image_size; i++)
	{
		output_filedata[i] = input_image_data[i];
	}

	int bitmap_data_offset = get_bitmap_data_offset((unsigned char*)output_filedata);

	for (int y = 0; y < image_height; y++)
	{
		for (int x = 0; x < image_width; x++)
		{
			int quantized_index = y * image_width + x;

			int color_index = quantized_data[quantized_index];

			int src_red = palette_data[color_index * 3];
			int src_green = palette_data[color_index * 3 + 1];
			int src_blue = palette_data[color_index * 3 + 2];

			int output_index = bitmap_data_offset + ((image_height - 1) - y) * (image_width * 3) + x * 3;
			output_filedata[output_index + 2] = src_blue;
			output_filedata[output_index + 1] = src_green;
			output_filedata[output_index + 0] = src_red;
		}
	}
	
	


	/*
	char * output_filedata = new char[35082];
	for (int i = 0; i < 35082; i++)
	{
		output_filedata[i] = input_image_data[i];
	}

	int bitmap_data_offset = get_bitmap_data_offset((unsigned char*)output_filedata);

	for (int y = 0; y < image_height; y++)
	{
		for (int x = 0; x < image_width; x++)
		{
			int index = y * (image_width * 3) + x * 3;
			
			int src_red = input_bitmap[index + 0];
			int src_green = input_bitmap[index + 1];
			int src_blue = input_bitmap[index + 2];

			int output_index = bitmap_data_offset + ((image_height - 1) - y) * (image_width * 3) + x * 3;
			
			
			output_filedata[output_index + 2] = src_blue;
			output_filedata[output_index + 1] = src_green;
			output_filedata[output_index + 0] = src_red;
		}
	}
	*/

	ofstream outbin(argv[3], ios::binary);
	outbin.write(reinterpret_cast <const char*> (output_filedata), 35082);
	outbin.close();
	

	delete[] palette_data;
	delete[] input_image_data;
	return 0;
}

