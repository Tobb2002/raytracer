/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "bvh_tree.hpp"

#define SAH_NUM_BUCKETS 13

#define COST_TRAVERSAL 1
#define COST_INTERSECT 1

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
  size_t axis = 0;  // axis > 2 --> no valid split
  size_t id = 0;
  bvh_box left;
  bvh_box right;
};

class SAH {
 public:
  explicit SAH(BVH_tree *tree);

  void split_middle(bvh_node_pointer *node);
  void split(bvh_node_pointer *node);

 private:
  /// @brief sorts the array beetween first and first + count INPLACE
  void sort(std::vector<uint>::iterator begin, const uint &count,
            const uint &axis);

  /// @brief Splits node a long longest axis
  void triangles_into_buckets(bvh_node_pointer *node, SAH_buckets *buckets);

  void split_middle_node(bvh_node_pointer *node);
  void split(bvh_node_pointer *node, const SAH_buckets &buckets,
             const split_point &splitp);

  split_point calc_min_split(bvh_node_pointer *node, SAH_buckets *buckets);
  bvh_box combine_box(SAH_buckets *buckets, const uint &axis, const uint &min,
                      const uint &max);
  void combine_ids(std::vector<uint> *result, const SAH_buckets &buckets,
                   const uint &axis, const uint &min, const uint &max);
  float get_surface_area(const bvh_box &box);

  BVH_tree *_tree;
  uint _max_triangles = 1;
};
