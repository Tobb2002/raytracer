/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <glm/glm.hpp>
#include <vector>
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

/// @brief Axis (X, Y, Z)
enum Axis { X, Y, Z };

/// @brief Struct for bounding volume node.
struct BVH_node {
  vec3 min = vec3(MAXFLOAT);
  vec3 max = vec3(-MAXFLOAT);
  bool leaf = true;
  uint first = 0; 
  uint count = 0;
};

/// @brief struct for BVH arrays data.
struct BVH_data {
  std::vector<Triangle> *triangles;
  std::vector<uint> triangle_ids;
  BVH_node *tree;
  uint size;
};

class BVH {
 private:
  BVH_data _data;

  /***** Funcitons *****/
  bool intersect_node_bool(uint id, Ray *ray);
  Intersection intersect_node(uint node_id, Ray *ray);
  Intersection intersect_leaf(uint id, Ray *ray);

  void update_intersection(Intersection * intersect, Intersection new_intersect);

  /// @brief swaps to triangle ids in BVHdata
  void swap_triangle(uint id1, uint id2);

  /// @brief get longest axis of bounding box.
  Axis get_longest_axis(uint node_id);

  void calculate_min(uint node_id);
  void calculate_max(uint node_id);

  void update_min(vec3 *min, vec3 min_value);
  void update_max(vec3 *min, vec3 min_value);

  uint get_lowest_hitbox();


  /// @brief sorts the array beetween first and first + count INPLACE
  void sort(uint first, uint count, Axis axis);

  /// @brief Splits node a long longest axis
  void split(uint node_id);

  uint _max_triangles = 10;

 public:

  
  /***** Functions *****/

  BVH();
  ~BVH();

  void build_tree(std::vector<Triangle> *triangles);

  Intersection intersect(Ray *ray);

  bool comp(uint id1, uint id2, Axis axis);

  /***** DEBUG *****/
  void print_node(uint id);
  void print_node_triangles(uint id);



  /***** Transformation *****/

  void apply_transform(mat4 t);

};
