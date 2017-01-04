#include <iostream>
#include "octree.h"
#include "data_sizes.h"

Octree::Octree()
{
    reset();
}

void Octree::reset()
{
    leaf_count = 0;
    deepest_leaf_level = TREE_DEPTH;
    
    for (int i = 0; i < TREE_DEPTH; i++)
    {
        reduce_list[i] = 0;
    }
    
    // Initialize the root node
    root = create_node(-1);
}

OctreeNode * Octree::create_node(int level)
{
    // Get a new node from the list
    OctreeNode * newnode = new OctreeNode(level, (level == TREE_DEPTH));

    if (newnode->is_leaf)
    {
        leaf_count++;
    }
    else if (level >= 0)
    {
        // Add to the top of the linked list
        newnode->nextnode = reduce_list[level];
        reduce_list[level] = newnode;
    }
    
    return newnode;
}

OctreeNode::OctreeNode(int level, bool leaf)
{
    level = level;
    is_leaf = leaf;
    
    npixels = 0;
    index = 0;
    redsum = greensum = bluesum = 0L;
    
    for (int i = 0; i < COLORBITS; i++)
    {
        child[i] = 0;
    }
}

void Octree::insert_color(uint8 red, uint8 green, uint8 blue)
{
    insert_color(root, red, green, blue, 0);
}

void Octree::insert_color(OctreeNode * cur_node, uint8 red, uint8 green, uint8 blue, int depth)
{
    if (cur_node->is_leaf)
    {
        cur_node->npixels++;
        cur_node->redsum += red;
        cur_node->greensum += green;
        cur_node->bluesum += blue;
    }
    else
    {
        int child_index = (((red >> (TREE_DEPTH-depth)) & 0x01) << 2) | (((green >> (TREE_DEPTH-depth)) & 0x01) << 1) | ((blue >> (TREE_DEPTH-depth)) & 0x01);
        if (cur_node->child[child_index] == 0)
        {
            cur_node->child[child_index] = create_node(depth);
        }
        insert_color(cur_node->child[child_index], red, green, blue, depth+1);
    }
}

int OctreeNode::reduce()
{
    ulong   sumred=0, sumgreen=0, sumblue=0;
    uint8    i, nchild=0;
    for (i = 0; i < COLORBITS; i++)
    {
        if (child[i])
        {
            nchild++;
            sumred += child[i]->redsum;
            sumgreen += child[i]->greensum;
            sumblue += child[i]->bluesum;
            npixels += child[i]->npixels;
            delete child[i];
            child[i] = 0;
        }
    }
    is_leaf = true;
    redsum = sumred;
    greensum = sumgreen;
    bluesum = sumblue;
        
    return nchild;
}

void Octree::reduce_step()
{
    OctreeNode * node = 0;
    
    // Find the deepest level that still has leaves in it
    while (reduce_list[deepest_leaf_level-1] == 0) {
        deepest_leaf_level--;
    }
    
    // Get the next node in the linked list
    node = reduce_list[deepest_leaf_level-1];
    
    // Move the linked list forward
    reduce_list[deepest_leaf_level-1] = reduce_list[deepest_leaf_level-1]->nextnode;

    // Reduce the node and get how many leaves were removed
    int leaves = node->reduce();
    
    // Update the leaf count
    leaf_count -= (leaves - 1);
}

void Octree::reduce(int desired_colors)
{
    while (leaf_count > desired_colors)
    {
        reduce_step();
    }
}

void Octree::make_palette_table(uint8 * output)
{
    int index = 0;
    make_palette_table(output, &index, root);
}

void Octree::make_palette_table(uint8 * output, int in_index)
{
    int index = in_index;
    make_palette_table(output, &index, root);
}

void Octree::make_palette_table(uint8 * output, int * index, OctreeNode * cur_node)
{
    int i;
    if (cur_node->is_leaf) {
        output[(*index) * 3 + 0] = (uint8)(cur_node->redsum / cur_node->npixels);
        output[(*index) * 3 + 1] = (uint8)(cur_node->greensum / cur_node->npixels);
        output[(*index) * 3 + 2] = (uint8)(cur_node->bluesum / cur_node->npixels);
        cur_node->index = (*index);
        (*index)++;
    }
    else {
        for (i = 0; i < COLORBITS; i++) {
            if (cur_node->child[i]) {
                make_palette_table(output, index, cur_node->child[i]);
            }
        }
    }
}

int Octree::find_color(uint8 red, uint8 green, uint8 blue)
{
    find_color(root, red, green, blue, 0);
}

int Octree::find_color(OctreeNode * cur_node, uint8 red, uint8 green, uint8 blue, int depth)
{
    if (cur_node->is_leaf)
    {
        return cur_node->index;
    }
    else
    {
        int child_index = (((red >> (TREE_DEPTH-depth)) & 0x01) << 2) | (((green >> (TREE_DEPTH-depth)) & 0x01) << 1) | ((blue >> (TREE_DEPTH-depth)) & 0x01);
        
        // If the child node we want is not there, find a child node that is close
        if (cur_node->child[child_index] == 0)
        {
            for (int i = 0; i < 8; i++)
            {
                /*
                if (child_index + i < 8 && cur_node->child[child_index + i] != 0)
                {
                    child_index = child_index + i;
                }
                else if (child_index - i >= 0 && cur_node->child[child_index - i] != 0)
                {
                    child_index = child_index - i;
                }
                */
                if (cur_node->child[i] != 0)
                {
                    child_index = i;
                }
            }
        }
        return find_color(cur_node->child[child_index], red, green, blue, depth+1);
    }
}

void Octree::walk_tree()
{
    root->walk();
}

void OctreeNode::walk()
{
    if (is_leaf)
    {
        std::cout << "leaf\n";
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            if (child[i] != 0)
            {
                std::cout << i << "\n";
                child[i]->walk();
            }
        }
    }
}