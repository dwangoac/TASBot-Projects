#include <iostream>
#include <fstream>
using namespace std;

int color_distance(unsigned char src_red, unsigned char src_green, unsigned char src_blue, unsigned char dest_red, unsigned char dest_green, unsigned char dest_blue)
{
	return (dest_red - src_red) * (dest_red - src_red) + (dest_green - src_green) * (dest_green - src_green) + (dest_blue - src_blue) * (dest_blue - src_blue);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " <palette> \n";
		return -1;
	}

	unsigned char * palette_data;
	palette_data = new unsigned char[256 * 3];

	// Clear out the palette
	for (int i = 0; i < 256; i++)
	{
		palette_data[i * 3 + 0] = 0;
		palette_data[i * 3 + 1] = 0;
		palette_data[i * 3 + 2] = 0;
	}

	// Read in the palette leaving the first color as black
	ifstream palette_file(argv[1], ios::in | ios::binary | ios::ate);
	if (palette_file.is_open())
	{
		streampos palette_size;
		palette_size = palette_file.tellg();

		if (palette_size > 255 * 3)
		{
			palette_size = 255 * 3;
		}
		
		palette_file.seekg(0, ios::beg);
		palette_file.read((char *)palette_data+3, palette_size);
		palette_file.close();
	}
	else
	{
		cout << "Unable to open palette";
		return -1;
	}
	
	// Convert palette to 15 bit
	for (int i = 0; i < 256; i++)
	{
		palette_data[i * 3 + 0] = palette_data[i * 3 + 0] & 0xF8;
		palette_data[i * 3 + 1] = palette_data[i * 3 + 1] & 0xF8;
		palette_data[i * 3 + 2] = palette_data[i * 3 + 2] & 0xF8;
	}

	// Output the palette
	for (int i = 0; i < 256; i++)
	{
		cout.put(palette_data[i * 3 + 0]);
		cout.put(palette_data[i * 3 + 1]);
		cout.put(palette_data[i * 3 + 2]);
	}

	// Quantize pixels one at a time
	while (true)
	{
		char red;
		char green;
		char blue;
		if (!std::cin.get(red))
		{
			return 0;
		}
		if (!std::cin.get(green))
		{
			return 0;
		}
		if (!std::cin.get(blue))
		{
			return 0;
		}

		int best_color = 0;
		int best_result = 50000000;
		for (int i = 0; i < 256; i++)
		{
			int distance = color_distance(red, green, blue, palette_data[(3 * i) + 0], palette_data[(3 * i) + 1], palette_data[(3 * i) + 2]);
			if (distance < best_result)
			{
				best_color = i;
				best_result = distance;
			}
		}

		cout.put(best_color);
	}

	return 0;
}

