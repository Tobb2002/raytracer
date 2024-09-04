/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "bvh_tree.hpp"
#include "ray.hpp"
#include "sah.hpp"
#include "triangle.hpp"

using glm::vec3;

enum Algorithm { AGRID, ASAH, ALBVH, AHLBVH, AMID };

struct Triangle_set {
  uint start_id;
  uint count;
};

struct Interval {
  float min;
  float max;
};

/// @brief struct for BVH arrays data.
struct BVH_data {
  std::vector<Triangle> *triangles;
  std::vector<uint> triangle_ids;
  BVH_tree tree;
  uint size;
};

class BVH {
 private:
  BVH_data _data;

  /***** Funcitons *****/

  /// @brief store the currently best intersection.
  Intersection _best_intersection;
  bool intersect_node_bool(BVH_node_data *node_data, const Ray &ray);
  void intersect_node(bvh_node_pointer *node, const Ray &ray);
  void intersect_node(uint id_flat, const Ray &ray);
  void intersect_leaf(BVH_node_data *node_data, const Ray &ray);

  bool update_intersection(Intersection *intersect,
                           const Intersection &new_intersect);

  /// @brief swaps to triangle ids in BVHdata
  void swap_triangle(bvh_node_pointer *node1, bvh_node_pointer *node2);

  uint get_lowest_hitbox();

  /// @brief calculate costs of given split
  float get_cost();

  uint _intersect_count = 0;

 public:
  BVH() {}

  void build_tree_axis(std::vector<Triangle> *triangles, Algorithm algorithm);
  void set_triangles(std::vector<Triangle> *triangles);

  /**
   * @brief Return best triangle intersection if found.
   *
   * @param ray
   * @return Intersection
   */
  Intersection intersect(const Ray &ray);

  /***** DEBUG *****/
  void print_node(bvh_node_pointer *node);
  void print_node_triangles(bvh_node_pointer *node);

  void update_boxes() { _data.tree.update_box(0); }

  /***** Transformation *****/

  /**
   * @brief Apply a transfromation to the Hierarchy
   *
   * @param t Transformation matrix
   */
  void apply_transform(const mat4 &t);
};
