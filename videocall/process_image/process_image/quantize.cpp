int color_distance(unsigned char src_red, unsigned char src_green, unsigned char src_blue, unsigned char dest_red, unsigned char dest_green, unsigned char dest_blue)
{
	return (dest_red - src_red) * (dest_red - src_red) + (dest_green - src_green) * (dest_green - src_green) + (dest_blue - src_blue) * (dest_blue - src_blue);
}

void quantize_bitmap(unsigned char * input_data, unsigned char * output_data, int image_size_x, int image_size_y, unsigned char * palette_data, int palette_size)
{
	for (int y = 0; y < image_size_y; y++)
	{
		for (int x = 0; x < image_size_x; x++)
		{
			int index = y*(image_size_x * 3) + x * 3;
			unsigned char red = input_data[index + 0];
			unsigned char green = input_data[index + 1];
			unsigned char blue = input_data[index + 2];

			int best_color = 0;
			int best_result = 50000000;
			for (int i = 0; i < palette_size; i++)
			{
				int distance = color_distance(red, green, blue, palette_data[(3 * i) + 0], palette_data[(3 * i) + 1], palette_data[(3 * i) + 2]);
				if (distance < best_result)
				{
					best_color = i;
					best_result = distance;
				}
			}

			output_data[y * image_size_x + x] = best_color;
		}
	}
}