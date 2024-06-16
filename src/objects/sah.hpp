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

};
