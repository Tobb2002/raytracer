/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "bvh_tree.hpp"
#include "lbvh.hpp"
#include <cstdint>

uint64_t LBVH::float_to_int(float f) {
  return static_cast<long>(f * 1000000);
}

uint64_t LBVH::split3(uint64_t i) {
}

uint64_t LBVH::get_morton_value(vec3 v) {
  uint64_t res = 0;

  res |= split3(v.x) | split3(v.y) << 1 | split3(v.z) << 2;

  return res;
}

void LBVH::build() {
}
