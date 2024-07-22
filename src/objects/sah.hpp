/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "bvh_tree.hpp"

#define SAH_NUM_BUCKETS 9

#define COST_TRAVERSAL 1
#define COST_INTERSECT 1

// #define SPLIT_LONGEST_AXIS

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
#ifdef SPLIT_LONGEST_AXIS
  SAH_bucket buckets[1][SAH_NUM_BUCKETS];
#else
  SAH_bucket buckets[3][SAH_NUM_BUCKETS];
#endif
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
  void built_on_treelets();

  /**
   * @brief split the list of nodes (treelets recursivly using sah)
   *
   * @param id_start id of first item
   * @param count number of items to consider for split.
   */
  void split_treelets(uint id_start, uint count, bvh_node_pointer *parent);

 private:
  /// @brief sorts the array beetween first and first + count INPLACE
  void sort(std::vector<uint>::iterator begin, const uint &count,
            const uint &axis);

  /// @brief Splits node a long longest axis
  void triangles_into_buckets(bvh_node_pointer *node, SAH_buckets *buckets);
  uint triangles_into_buckets_axis(bvh_node_pointer *node,
                                   SAH_buckets *buckets);

  void split_middle_node(bvh_node_pointer *node);
  void split(bvh_node_pointer *node, const SAH_buckets &buckets,
             const split_point &splitp);

  split_point calc_min_split(bvh_node_pointer *node, SAH_buckets *buckets);
  split_point calc_min_split(bvh_node_pointer *node, SAH_buckets *buckets,
                             uint axis);

  split_point calc_min_split_treelets(uint id_start, uint count);

  bvh_box combine_box(SAH_buckets *buckets, const uint &axis, const uint &min,
                      const uint &max);
  bvh_box combine_box_treelets(uint id_start, uint count);
  void combine_ids(std::vector<uint> *result, const SAH_buckets &buckets,
                   const uint &axis, const uint &min, const uint &max);
  float get_surface_area(const bvh_box &box);

  BVH_tree *_tree;
  uint _max_triangles = 1;
};
