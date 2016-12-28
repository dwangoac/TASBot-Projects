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
    
    bool recieving = true;
    while (recieving)
    {
        uint8 color[3] = {0,0,0};
        for (int i = 0; i < 3; i++)
        {
            char temp;
            if (!std::cin.get(temp))
            {
                recieving = false;
                break;
            }
            color[i] = temp;
        }

        foo.insert_color(color[0], color[1], color[2]);
    }

    //cout << foo.leaf_count << "\n";
    
    foo.reduce(256);
   // cout << foo.leaf_count << "\n";
    
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
    
    return 0;
}

