/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "triangle.hpp"

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
  bool is_leaf;
};

union bvh_node {
  bvh_node_pointer pointer;
  bvh_node_flat flat;
};

class BVH_tree {
 public:
  BVH_tree() {}
  explicit BVH_tree(BVH_node_data root_data, std::vector<Triangle>* triangles);
  BVH_tree(const BVH_tree& old_tree);
  BVH_tree& operator=(const BVH_tree& old_tree);
  bvh_node_pointer* copy_node(bvh_node_pointer* old_node);
  ~BVH_tree();

  void set_triangles(std::vector<Triangle>* triangles);

  std::vector<Triangle>* get_triangle_vec();

  /// inserts data to first free child (left, right) ASSERTION if both full
  bvh_node_pointer* insert_child(BVH_node_data data, bvh_node_pointer* node);

  bvh_node_pointer* get_root();
  bvh_node_flat* get_root_flat();

  bvh_node_pointer* get_left(bvh_node_pointer* node);
  bvh_node_pointer* get_right(bvh_node_pointer* node);

  BVH_node_data* get_data(bvh_node_pointer* node);

  // --------------------------------------------------------------------------
  // operations

  void calculate_min(bvh_node_pointer* node);
  void calculate_max(bvh_node_pointer* node);
  void calculate_bounds(bvh_node_pointer* node);

  void update_min(vec3* min, const vec3& min_value);
  void update_max(vec3* min, const vec3& min_value);
  void update_bounds(vec3* min, const vec3& min_value, vec3* max,
                     const vec3& max_value);

  Triangle* get_triangle(uint id);

  /// @brief get longest axis of bounding box.
  uint get_longest_axis(bvh_node_pointer* node);

  bvh_box update_box(bvh_node_pointer* node);
  void update_boxes();

  // operations on flattend tree

  uint get_left(uint id_flat);
  uint get_right(uint id_flat);

  BVH_node_data* get_data(uint id_flat);
  bvh_node_flat* get_node(uint id_flat);

  bool is_leaf(bvh_node_pointer* node);
  void free_triangles(bvh_node_pointer* node);

  void print_inorder();

  /**
   * @brief Saves the tree content in depth first search order into an array.
   */
  void flatten_tree();
  // void build_from_flattened();

 private:
  void destroy_node(bvh_node_pointer* node);
  void destroy_tree();

  uint flatten_node(bvh_node_pointer* node);

  std::vector<bvh_node_flat> _triangles_flat;
  bvh_node_pointer* root = nullptr;
  std::vector<Triangle>* _triangles;
};
