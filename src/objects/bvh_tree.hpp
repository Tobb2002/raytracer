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
  std::vector<uint> triangles_ids;
};

struct node {
  BVH_node_data data;
  node *parent = nullptr;
  node *left = nullptr;
  node *right = nullptr;
};


struct BVH_node_flat {
  BVH_node_data data;
  uint offset_right;
};

class BVH_tree{
 public:
  BVH_tree() {}
  BVH_tree(BVH_node_data root_data);
  ~BVH_tree();

  /// inserts data to first free child (left, right) ASSERTION if both full
  void insert_child(BVH_node_data data, node *node);

  node* get_left(node* node);
  node* get_right(node* node);

  BVH_node_data* get_data(node* node);

  bool is_leaf(node* node);

  void print_inorder();


  void flatten();
  //void build_from_flattened();
 private:
  void destroy_node(node *node);
  void destroy_tree();

  std::vector<uint> triangles_flat;
  node *root = nullptr;
};

