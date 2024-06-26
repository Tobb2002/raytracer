/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "lbvh.hpp"

#include <cstdint>
#include <glm/gtx/string_cast.hpp>

#include "bvh_tree.hpp"

LBVH::LBVH(BVH_tree *tree) { _tree = tree; }

uint32_t LBVH::float_to_int(float f) {
  if (f < 0 || f > 1) {
    throw std::runtime_error("Float value is not normalized.");
  }
  return static_cast<uint32_t>(f * (glm::pow(2, GRID_SIZE) - 1));
}

uint64_t LBVH::split3(uint32_t i) {
  uint64_t res = i & 0x1fffff;  // only consider last 21 bits

  // for explenation
  // see:https://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/
  //
  res =
      (res | res << 32) &
      0x1f00000000ffff;  // shift left 32 bits, OR with self, and
                         // 00011111000000000000000000000000000000001111111111111111
  res =
      (res | res << 16) &
      0x1f0000ff0000ff;  // shift left 32 bits, OR with self, and
                         // 00011111000000000000000011111111000000000000000011111111
  res =
      (res | res << 8) &
      0x100f00f00f00f00f;  // shift left 32 bits, OR with self, and
                           // 0001000000001111000000001111000000001111000000001111000000000000
  res =
      (res | res << 4) &
      0x10c30c30c30c30c3;  // shift left 32 bits, OR with self, and
                           // 0001000011000011000011000011000011000011000011000011000100000000
  res = (res | res << 2) & 0x1249249249249249;

  return res;
}

uint64_t LBVH::get_morton_value(vec3 v) {
  uint64_t res = 0;

  res |= split3(float_to_int(v.x)) | split3(float_to_int(v.y)) << 1 |
         split3(float_to_int(v.z)) << 2;

  return res;
}

void LBVH::build() {}
