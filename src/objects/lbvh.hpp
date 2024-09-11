/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#include "bvh_tree.hpp"
#include "morton.hpp"

using glm::vec3;

// GRID_SIZE: binary length of the highest gridzell index
// GRID_SIZE|#grid cells
// 1|2 , 2|4, 3|8, 4|16, 5|32, 6|64, 7|128, 8|256, 9|512, 10|1024, 11|2048,
// 12|4096
// Grid size time 3 should be less than 64 to fit the morton code into 64 bit
// integer
#define GRID_SIZE 10

// number of first morton code bits to be identical in the same treelet
// 0 = only use lbvh since just one treelet get's added
#define TREELET_BITS 15

struct morton_data {
  uint triangle_id;
  uint32_t morton_code;
};

class LBVH {
 public:
  explicit LBVH(BVH_tree *tree);

  /**
   * @brief Build tree from sorted triangles in root node.
   */
  void build();

  /**
   * @brief build lbvh for every treelet.
   */
  void build_treelets();

 private:
  /**
   * @brief Sort the triangles according to their position (morton codes)
   */
  void sort();

  void generate_morton_codes();

  /**
   * @brief Split current node into two childs at given id.
   *
   * @param node
   * @param id
   */
  void split(bvh_node_pointer *node, uint id);

  /**
   * @brief split node into treelets and free node afterwards.
   *
   * @param node
   */
  void add_treelets(bvh_node_pointer *node);

  /**
   * @brief Split current node at the first differing bit of the triangles
   * morton codes.
   *
   * @param node
   * @param current_bit
   */
  void split_first_bit(bvh_node_pointer *node, uint current_bit);

  BVH_tree *_tree;
  // Saves morton code for triangle with id i at index i
  Morton _morton = Morton(nullptr, GRID_SIZE);
};
