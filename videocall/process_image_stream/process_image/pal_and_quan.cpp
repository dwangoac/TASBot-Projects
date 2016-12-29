#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "octree.h"
#include "data_sizes.h"

using namespace std;

int main()
{
    Octree foo;
    uint8 video_frame[120*104*3];
    
    uint8 color[3] = {0,0,0};
    for (int i = 0; i < 120*104*3; i++)
    {
        char temp;
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
    
    for (int i = 0; i < 256*3; i++)
    {
        cout.put(palette[i]);
    }
    
    for (int i = 0; i < 120*104*3; i+=3)
    {
        cout.put(foo.find_color(video_frame[i],video_frame[i+1],video_frame[i+2]));
    }

    
    return 0;
}

