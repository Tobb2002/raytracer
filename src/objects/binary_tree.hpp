/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
#include <vector>

using glm::vec3;

struct bvh_box {
  vec3 min = vec3(0);
  vec3 max = vec3(0);
};

/// @brief Struct for bounding volume node.
struct BVH_node_data {
  bvh_box bounds = bvh_box(vec3(MAXFLOAT), vec3(-MAXFLOAT));
  std::vector<uint> triangle_ids;
};

struct bvh_node {
  BVH_node_data data;
  bvh_node *parent;
  bvh_node *left = nullptr;
  bvh_node *right = nullptr;
};

struct BVH_node_flat {
  BVH_node_data data;
  uint offset_right;
};

class BVH_tree {
 public:
  BVH_tree();
  explicit BVH_tree(bvh_node *root_node);
  ~BVH_tree();

  /// inserts data to first free child (left right) ASSERTION if both full
  void insert(BVH_node_data data, bvh_node *node);

  void print_inorder();

  void flatten();
  // void build_from_flattened();
 private:
  std::vector<uint> triangles_flat;
  bvh_node *root = nullptr;
};
