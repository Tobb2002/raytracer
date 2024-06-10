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

struct bvh_node_pointer {
  BVH_node_data data;
  bvh_node_pointer* parent = nullptr;
  bvh_node_pointer* left = nullptr;
  bvh_node_pointer* right = nullptr;
};

struct bvh_node_flat {
  BVH_node_data data;
  uint offset_right;
};

union bvh_node {
  bvh_node_pointer pointer;
  bvh_node_flat flat;
};

class BVH_tree {
 public:
  BVH_tree() {}
  explicit BVH_tree(BVH_node_data root_data);
  BVH_tree(const BVH_tree& old_tree);
  BVH_tree& operator=(const BVH_tree& old_tree);
  bvh_node_pointer* copy_node(bvh_node_pointer* old_node);
  ~BVH_tree();

  /// inserts data to first free child (left, right) ASSERTION if both full
  bvh_node_pointer* insert_child(BVH_node_data data, bvh_node_pointer* node);

  bvh_node_pointer* get_root();
  bvh_node_flat* get_root_flat();

  bvh_node_pointer* get_left(bvh_node_pointer* node);
  bvh_node_pointer* get_right(bvh_node_pointer* node);

  BVH_node_data* get_data(bvh_node_pointer* node);

  // operations on flattend tree

  bvh_node_flat* get_left(bvh_node_flat* node);
  bvh_node_flat* get_right(bvh_node_flat* node);

  bvh_node_flat* get_data(bvh_node_flat* node);

  bool is_leaf(bvh_node_pointer* node);
  void free_triangles(bvh_node_pointer* node);

  void print_inorder();

  void flatten();
  // void build_from_flattened();
 private:
  void destroy_node(bvh_node_pointer* node);
  void destroy_tree();

  std::vector<bvh_node_flat> triangles_flat;
  bvh_node_pointer* root = nullptr;
};
