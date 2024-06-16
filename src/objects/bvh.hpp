/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "bvh_tree.hpp"
#include "sah.hpp"
#include "ray.hpp"
#include "triangle.hpp"

using glm::vec3;

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
  bool intersect_node_bool(bvh_node_pointer *node, const Ray &ray);
  Intersection intersect_node(bvh_node_pointer *node, const Ray &ray);
  Intersection intersect_leaf(bvh_node_pointer *node, const Ray &ray);

  bool update_intersection(Intersection *intersect,
                           const Intersection &new_intersect);

  /// @brief swaps to triangle ids in BVHdata
  void swap_triangle(bvh_node_pointer *node1, bvh_node_pointer *node2);




  uint get_lowest_hitbox();

  /// @brief sorts the array beetween first and first + count INPLACE
  void sort(std::vector<uint>::iterator begin, const uint &count,
            const uint &axis);

  /// @brief Splits node a long longest axis
  void triangles_into_buckets(bvh_node_pointer *node, SAH_buckets *buckets);

  void split_middle(bvh_node_pointer *node);
  void split_middle_node(bvh_node_pointer *node);
  void split_SAH(bvh_node_pointer *node);
  void split(bvh_node_pointer *node, const SAH_buckets &buckets,
             const split_point &splitp);

  /// @brief calculate costs of given split
  float get_cost();

  uint _max_triangles = 1;

  uint _intersect_count = 0;

 public:
  BVH() {}
  ~BVH() {}

  void build_tree_axis(std::vector<Triangle> *triangles);
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
  /***** SAH ******/

  split_point calc_min_split(bvh_node_pointer *node, SAH_buckets *buckets);
  bvh_box combine_box(SAH_buckets *buckets, const uint &axis, const uint &min,
                      const uint &max);
  void combine_ids(std::vector<uint> *result, const SAH_buckets &buckets,
                   const uint &axis, const uint &min, const uint &max);
  float get_surface_area(const bvh_box &box);

  /***** Transformation *****/

  /**
   * @brief Apply a transfromation to the Hierarchy
   *
   * @param t Transformation matrix
   */
  void apply_transform(const mat4 &t);
};
