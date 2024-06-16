/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <cstdint>
#include <glm/glm.hpp>

#include "bvh_tree.hpp"

using glm::vec3;

// number of zells to divide space in one dimension
#define GRID_SIZE 1024  // 2^10 --> morton code fits in uint32

struct morton_data {
  uint triangle_id;
  uint32_t morton_code;
};

class LBVH {
 public:
  explicit LBVH(BVH_tree *tree);

  void build();

 private:
  /**
   * @brief convert float to an integer number considering decimal places.
   *
   * @param f
   */
  uint64_t float_to_int(float f);

  uint64_t split3(uint64_t i);

  /**
   * @brief return morton value for a given vector.
   *
   * @param v input vector.
   */
  uint64_t get_morton_value(vec3 v);

  BVH_tree *_tree;
};
