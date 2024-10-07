/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "morton.hpp"

#include <algorithm>
#include <boost/lambda/bind.hpp>
#include <cstdint>
#include <execution>

Morton::Morton(std::vector<Triangle> *triangles, uint grid_bits) {
  _triangles = triangles;
  _grid_bits = grid_bits;
  _morton_size = grid_bits * 3;
}
void Morton::initialize_grid_bits(std::vector<Triangle> *triangles,
                                  uint grid_bits) {
  _triangles = triangles;
  _grid_bits = grid_bits;
  _morton_size = grid_bits * 3;
}
void Morton::initialize_grid_size(std::vector<Triangle> *triangles,
                                  uint grid_size) {
  _triangles = triangles;
  _grid_bits = glm::ceil(glm::log2(static_cast<float>(grid_size)));
  _morton_size = _grid_bits * 3;
}

bool comp(std::vector<uint64_t> *morton_codes, uint id1, uint id2) {
  if (morton_codes->at(id1) < morton_codes->at(id2)) {
    return true;
  }
  return false;
}

void Morton::sort(std::vector<uint> *triangle_ids) {
  namespace boo = boost::lambda;

  // BVH_node_data *data = _tree->get_data(_tree->get_root());

  // std::vector<uint>::iterator it = data->triangle_ids.begin();

  std::vector<uint>::iterator it = triangle_ids->begin();
  std::sort(std::execution::par_unseq, it, it + triangle_ids->size(),
            boo::bind(&comp, &_morton_codes, boo::_1, boo::_2));
}

void Morton::generate_morton_codes(std::vector<uint> *triangle_ids,
                                   const bvh_box &bounds) {
  for (size_t i = 0; i < triangle_ids->size(); i++) {
    Triangle *t = _triangles->data() + i;
    // get normalized triangle position dependent on bounding box
    vec3 pos = t->get_pos();
    vec3 pos_normalized =
        glm::abs((pos - bounds.min) / (bounds.max - bounds.min));

    // save respective morten code trianlge with id i -> morton code at index i
    _morton_codes.push_back(get_morton_value(pos_normalized));
  }
}
void Morton::build(std::vector<uint> *triangle_ids, const bvh_box &bounds) {
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();

  generate_morton_codes(triangle_ids, bounds);
  sort(triangle_ids);

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  float time_building =
      (std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
           .count()) /
      1000000.0;
  std::cout << "------------------------------------------------\n";
  std::cout << "Time for building morton codes (sec) = ";
  std::cout << time_building << "\n";
  std::cout << "------------------------------------------------\n";
}

uint64_t Morton::get_code(uint id) { return _morton_codes.at(id); }
uint Morton::get_morton_size() { return _morton_size; }

uint64_t Morton::get_morton_value(vec3 v) {
  uint64_t res = 0;

  res |= split3(float_to_int(v.x)) | split3(float_to_int(v.y)) << 1 |
         split3(float_to_int(v.z)) << 2;

  return res;
}

uint64_t Morton::get_value(vec3 index) {
  // check if index inside maximum grid size
  uint64_t res = 0;

  res |= split3(static_cast<uint32_t>(index.x)) |
         split3(static_cast<uint32_t>(index.y)) << 1 |
         split3(static_cast<uint32_t>(index.z)) << 2;

  return res;
}

uint32_t Morton::float_to_int(float f) {
  if (f < 0 || f > 1) {
    throw std::runtime_error("Float value is not normalized.");
  }
  return static_cast<uint32_t>(f * (glm::pow(2, _grid_bits) - 1));
}

uint64_t Morton::split3(uint32_t i) {
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
