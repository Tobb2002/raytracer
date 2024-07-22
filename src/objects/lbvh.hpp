/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#include "bvh_tree.hpp"

using glm::vec3;

// number of zells to divide space in one dimension
// Grid size time 3 should be less than 64 to fit the morton code into 64 bit
// integer
#define GRID_SIZE 10

// number of bits for the morton code
#define MORTON_SIZE GRID_SIZE * 3

// number of first morton code bits to be identical in the same treelet
// 0 = only use lbvh since just one treelet get's added
#define TREELET_BITS 8

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
   * @brief Convert float to an integer number considering decimal places.
   *
   * @param f Needs to be normalized (f in [0,1])
   */
  uint32_t float_to_int(float f);

  /**
   * @brief seperate bits such that there are 2 zeros between every data bit.
   *
   * @param i Input value: can have at maximum 21 data bits such that the result
   * does not overflow. If input has more bits they are not taken into account.
   * @return
   */
  uint64_t split3(uint32_t i);

  /**
   * @brief return morton value for a given vector.
   *
   * @param v input vector. with values between 0,1
   */
  uint64_t get_morton_value(vec3 v);

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
  std::vector<uint64_t> _morton_codes;
};
