/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "ray.hpp"
#include "triangle.hpp"

#define SAH_NUM_BUCKETS 12

#define COST_TRAVERSAL 1
#define COST_INTERSECT 1

using glm::vec3;

struct Triangle_set {
  uint start_id;
  uint count;
};

struct Interval {
  float min;
  float max;
};

struct bvh_box {
  vec3 min = vec3(0);
  vec3 max = vec3(0);
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

/**
 * @class SAH_bucket
 * @brief Sturct holding all the information of a bucket
 *
 */
struct SAH_bucket {
  std::vector<uint> ids;
  bvh_box box;
  float cost;
};

/// @brief Struct containing two-dimensional array of all buckets
struct SAH_buckets {
  SAH_bucket buckets[3][SAH_NUM_BUCKETS];
};

struct split_point {
  size_t axis = 0;
  size_t left_count = 0;
  bvh_box left;
  bvh_box right;
};


/// @brief struct for BVH arrays data.
struct BVH_data {
  std::vector<Triangle> *triangles;
  std::vector<uint> triangle_ids;
  std::vector<BVH_node> tree;
  uint size;
};

class BVH {
 private:
  BVH_data _data;

  /***** Funcitons *****/
  bool intersect_node_bool(const uint &id, const Ray& ray);
  Intersection intersect_node(const uint &node_id, const Ray& ray);
  Intersection intersect_leaf(const uint &id, const Ray& ray);

  bool update_intersection(Intersection * intersect,
                           const Intersection &new_intersect);

  /// @brief swaps to triangle ids in BVHdata
  void swap_triangle(const uint &id1, const uint &id2);

  /// @brief get longest axis of bounding box.
  Axis get_longest_axis(const uint &node_id);

  bvh_box update_box(const uint &node_id);

  void calculate_min(const uint &node_id);
  void calculate_max(const uint &node_id);

  void update_min(vec3 *min, const vec3 &min_value);
  void update_max(vec3 *min, const vec3 &min_value);

  uint get_lowest_hitbox();


  /// @brief sorts the array beetween first and first + count INPLACE
  void sort(const uint &first, const uint &count, const Axis &axis);

  /// @brief Splits node a long longest axis
  void split_middle(const uint &node_id);
  void triangles_into_buckets(uint node_id, SAH_buckets *buckets);

  void split_SAH(const uint &node_id);
  void split(const uint &node_id, const size_t &axis, const uint &left_amount);



  /// @brief calculate costs of given split
  float get_cost();

  uint _max_triangles = 2;

  uint _intersect_count = 0;

 public:
  BVH() { }
  ~BVH() { }

  void build_tree_axis(std::vector<Triangle> *triangles);
  void set_triangles(std::vector<Triangle> *triangles);

  /**
   * @brief Return best triangle intersection if found.
   * 
   * @param ray 
   * @return Intersection 
   */
  Intersection intersect(const Ray& ray);

  /***** DEBUG *****/
  void print_node(const uint &id);
  void print_node_triangles(const uint &id);

  /***** SAH ******/

  split_point calc_min_split(const uint &node_id, SAH_buckets *buckets);
  bvh_box combine_box(SAH_buckets *buckets, const uint &axis, const uint &min, const uint &max);
  float get_surface_area(const bvh_box& box);


  /***** Transformation *****/

  /**
   * @brief Apply a transfromation to the Hierarchy
   *
   * @param t Transformation matrix
   */
  void apply_transform(const mat4 &t);
  void update_boxes();
};
