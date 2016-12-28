#ifndef OCTREE_H
#define OCTREE_H

#include "data_sizes.h"

#define TREE_DEPTH 6
#define COLORBITS   8

class OctreeNode
{
public:
  OctreeNode(int level, bool leaf);
  
  bool    is_leaf;
  int     level;
  uint8    index;
  ulong   npixels;
  ulong   redsum, greensum, bluesum;
  OctreeNode *child[8];
  OctreeNode *nextnode;
  
  int reduce();

};


class Octree
{
public:
  Octree();
  
  OctreeNode *reduce_list[TREE_DEPTH];

  OctreeNode *root;
  
  void reduce_step();
  void reduce(int desired_colors);
  
  OctreeNode * create_node(int level);
  void insert_color(uint8 red, uint8 green, uint8 blue);
  void insert_color(OctreeNode * cur_node, uint8 red, uint8 green, uint8 blue, int depth);
  void reset();
  void make_palette_table(uint8 * output);
  void make_palette_table(uint8 * output, int * index, OctreeNode * cur_node);
  
  int leaf_count;
  int deepest_leaf_level;
};

#endif // OCTREE_H