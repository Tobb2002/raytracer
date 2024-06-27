/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "lbvh.hpp"

#include <cstdint>
#include <glm/gtx/string_cast.hpp>
#include <boost/lambda/bind.hpp>
#include <algorithm>

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

  res = (res | res << 32) & 0x1f00000000ffff;
  res = (res | res << 16) & 0x1f0000ff0000ff;
  res = (res | res << 8) & 0x100f00f00f00f00f;
  res = (res | res << 4) & 0x10c30c30c30c30c3;
  res = (res | res << 2) & 0x1249249249249249;

  return res;
}

uint64_t LBVH::get_morton_value(vec3 v) {
  uint64_t res = 0;

  res |= split3(float_to_int(v.x)) | split3(float_to_int(v.y)) << 1 |
         split3(float_to_int(v.z)) << 2;

  return res;
}

void LBVH::generate_morton_codes() {
  for (size_t i = 0; i < _tree->get_triangle_vec()->size(); i++) {
    Triangle *t = _tree->get_triangle(i);
    // get normalized triangle position dependent on bounding box
    vec3 pos = t->get_pos();
    vec3 bounds_min = _tree->get_data(_tree->get_root())->bounds.min;
    vec3 bounds_max = _tree->get_data(_tree->get_root())->bounds.max;

    vec3 pos_normalized = (pos - bounds_min) / (bounds_max - bounds_min);

    // save respective morten code trianlge with id i -> morton code at index i
    _morton_codes.push_back(get_morton_value(pos_normalized));
  }
}

bool comp(std::vector<uint64_t> *morton_codes, uint id1, uint id2) {
  if (morton_codes->at(id1) < morton_codes->at(id2)) {
    return true;
  }
  return false;
}

void LBVH::sort() {
  namespace boo = boost::lambda;

  BVH_node_data *data = _tree->get_data(_tree->get_root());
  
  std::vector<uint>::iterator it = data->triangle_ids.begin();
  std::sort(it, it + data->triangle_ids.size(),
            boo::bind(&comp, &_morton_codes, boo::_1, boo::_2));
}

void LBVH::split_first_bit() {}

void LBVH::build() {
  generate_morton_codes();
  sort();
  
  for (uint id : _tree->get_data(_tree->get_root())->triangle_ids) {
    std::cout << _morton_codes.at(id) << "\n";
  }
  split_first_bit();
}
