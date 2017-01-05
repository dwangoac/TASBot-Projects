#include <iostream>
#include <stdlib.h>

#include "octree.h"
#include "data_sizes.h"
#include "trans.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "usage: " << argv[0] << " <palette interval>\n";
    }
    
    int interval = atoi(argv[1]);
    
    if (interval < 1)
    {
        cout << "Invalid palette interval\n";
        return -1;
    }
  
    bool highmem = true;
    uint8 quan_data[VFRAME_SIZE];
    char temp;

    Octree pal1;
  
    uint8 video_frame[VFRAME_RGB_SIZE];

    // Read in the first frame
    for (int i = 0; i < VFRAME_RGB_SIZE; i++)
    {
        if (!std::cin.get(temp))
        {
            return 0;
        }
        video_frame[i] = temp;
        
        
        if (i % 3 == 2)
        {
            pal1.insert_color(video_frame[i-2], video_frame[i-1], video_frame[i]);
        }
    }
    
    // Use it to generate a palette
    pal1.reduce(254);

    uint8 palette[256*3];
    for (int i = 0; i < 256*3; i++)
    {
        palette[i] = 0;
    }
    pal1.make_palette_table(palette, 1);
    
    palette[0*3+0] = 0;
    palette[0*3+1] = 0;
    palette[0*3+2] = 0;
    palette[255*3+0] = 255;
    palette[255*3+1] = 255;
    palette[255*3+2] = 255;

    // Quantize the first frame
    for (int i = 0; i < VFRAME_SIZE; i++)
    {
        quan_data[i] = pal1.find_color(video_frame[i*3+0],video_frame[i*3+1],video_frame[i*3+2]);
    }
    
    trans_frame(quan_data, highmem, !highmem);
    highmem = !highmem;
    trans_palette(palette, highmem);
    
    
    
    while (true)
    {
        Octree pal2;
        for (int frame = 0; frame < interval; frame++)
        {
            for (int i = 0; i < VFRAME_RGB_SIZE; i++)
            {
                if (!std::cin.get(temp))
                {
                    return 0;
                }
                video_frame[i] = temp;
                
                if (i % 3 == 2)
                {
                    // Insert into the new octree
                    pal2.insert_color(video_frame[i-2], video_frame[i-1], video_frame[i]);
                    
                    // Use the old octree to find this color
                    quan_data[i] = pal1.find_color(video_frame[i-2],video_frame[i-1],video_frame[i]);
                }
                                
            }
            
            // Transmit this frame
            if (frame == 0)
            {
                trans_frame(quan_data, highmem, !highmem);
                highmem = !highmem;
                trans_palette(palette, highmem);
            }
            else
            {
                trans_frame(quan_data, highmem, highmem);
                highmem = !highmem;
            }
        }
        
        // Copy over the new octree
        pal1 = pal2;
        
        // Convert it to a palette
        pal1.reduce(254);
    
        for (int i = 0; i < 256*3; i++)
        {
            palette[i] = 0;
        }
        pal1.make_palette_table(palette, 1);
        
        palette[0*3+0] = 0;
        palette[0*3+1] = 0;
        palette[0*3+2] = 0;
        palette[255*3+0] = 255;
        palette[255*3+1] = 255;
        palette[255*3+2] = 255;
    }

    return 0;
}