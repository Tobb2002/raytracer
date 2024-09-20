/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "bvh_tree.hpp"

#define SAH_NUM_BUCKETS 12

// number of triangles at which array gets split in the middle
#define MIN_SAH_SPLIT 2

#define COST_TRAVERSAL 2
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
  size_t axis = 3;  // axis > 2 --> no valid split
  size_t id = 0;
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
  void split_treelets(bvh_node_pointer *node);

 private:
  /// @brief sorts the array beetween first and first + count INPLACE
  void sort(std::vector<uint>::iterator begin, const uint &count,
            const uint &axis);

  void sort_treelets(std::vector<uint>::iterator begin, const uint &count,
                     const uint &axis);

  /// @brief Splits node a long longest axis
  void triangles_into_buckets(bvh_node_pointer *node, SAH_buckets *buckets);
  uint triangles_into_buckets_axis(bvh_node_pointer *node,
                                   SAH_buckets *buckets);

  void treelets_into_buckets(bvh_node_pointer *node, SAH_buckets *buckets);
  uint treelets_into_buckets_axis(bvh_node_pointer *node, SAH_buckets *buckets);

  void split_middle_node(bvh_node_pointer *node);
  void split_middle_node_treelets(bvh_node_pointer *node);
  void split(bvh_node_pointer *node, const SAH_buckets &buckets,
             const split_point &splitp);

  split_point calc_min_split(bvh_node_pointer *node, SAH_buckets *buckets);
  split_point calc_min_split(bvh_node_pointer *node, SAH_buckets *buckets,
                             uint axis);

  split_point calc_min_split_treelets(bvh_node_pointer *node,
                                      SAH_buckets *buckets);
  split_point calc_min_split_treelets(bvh_node_pointer *node,
                                      SAH_buckets *buckets, uint axis);

  bvh_box combine_box(const SAH_buckets &buckets, const uint &axis,
                      const uint &min, const uint &max);
  bvh_box combine_box_treelets(bvh_node_pointer *node);

  bvh_box union_box(bvh_box a, bvh_box b);
  void combine_ids(std::vector<uint> *result, const SAH_buckets &buckets,
                   const uint &axis, const uint &min, const uint &max);
  float get_surface_area(const bvh_box &box);

  BVH_tree *_tree;
  uint _max_triangles = 1;
};
