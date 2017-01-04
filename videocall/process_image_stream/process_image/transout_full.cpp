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
  
    uint8 video_frame[VFRAME_RGB_SIZE * interval];
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
    uint8 quan_data[VFRAME_SIZE];
    char temp;
    while (true)
    {
        Octree foo;
        for (int frame = 0; frame < interval; frame++)
        {
            for (int i = 0; i < VFRAME_RGB_SIZE; i++)
            {
                if (!std::cin.get(temp))
                {
                    return 0;
                }
                video_frame[frame * VFRAME_RGB_SIZE + i] = temp;
                
                
                if (i % 3 == 2)
                {
                    foo.insert_color(video_frame[frame * VFRAME_RGB_SIZE + i-2], video_frame[frame * VFRAME_RGB_SIZE + i-1], video_frame[frame * VFRAME_RGB_SIZE + i]);
                }
            }
        }
        
        foo.reduce(254);
    
        uint8 palette[256*3];
        for (int i = 0; i < 256*3; i++)
        {
            palette[i] = 0;
        }
        foo.make_palette_table(&palette[3]);
        palette[255*3+0] = 255;
        palette[255*3+1] = 255;
        palette[255*3+2] = 255;
        
        for (int frame = 0; frame < interval; frame++)
        {
            for (int i = 0; i < VFRAME_SIZE; i++)
            {
                quan_data[i] = foo.find_color(video_frame[frame * VFRAME_RGB_SIZE + i*3+0],video_frame[frame * VFRAME_RGB_SIZE + i*3+1],video_frame[frame * VFRAME_RGB_SIZE + i*3+2]);
            }
            
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