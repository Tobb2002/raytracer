/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#include "bvh_tree.hpp"
#include "triangle.hpp"

using glm::vec3;

class Morton {
 public:
  Morton() {}
  explicit Morton(std::vector<Triangle> *triangles, uint grid_bits);

  void build(std::vector<uint> *triangle_ids, const bvh_box &bounds);

  /**
   * @brieg get morton code to specific id.
   */
  uint64_t get_code(uint id);
  uint get_morton_size();
  void initialize_grid_bits(std::vector<Triangle> *triangles, uint grid_bits);
  void initialize_grid_size(std::vector<Triangle> *triangles, uint grid_size);

  /// @brief calculate moroton code for a given cell index
  /// @param index should only contain integer values
  uint64_t get_value(vec3 index);

 private:
  /**
   * @brief Sort the triangles according to their position (morton codes)
   */
  void sort(std::vector<uint> *triangle_ids);

  /**
   * @brief generate mortoncodes that
   */
  void generate_morton_codes(std::vector<uint> *triangle_ids,
                             const bvh_box &bounds);

  /**
   * @brief return morton value for a given vector.
   *
   * @param v input vector. with values between 0,1
   */
  uint64_t get_morton_value(vec3 v);

  /**
   * @brief Convert float to an integer number considering decimal places.
   *
   * @param f Needs to be normalized (f in [0,1])
   */
  uint32_t float_to_int(float f);

  /**
   * @brief seperate bits such that there are 2 zeros between every data bit.
   *
   * @param i Input value: can have at maximum 21 data bits such that the result
   * does not overflow. If input has more bits they are not taken into account.
   * @return
   */
  uint64_t split3(uint32_t i);

  std::vector<uint64_t> _morton_codes;
  std::vector<Triangle> *_triangles;

  // GRID_SIZE|#grid cells
  // 1|2 , 2|4, 3|8, 4|16, 5|32, 6|64, 7|128, 8|256, 9|512, 10|1024, 11|2048,
  // 12|4096
  /// @brief number of bits that the maximum gridcell uses.
  uint _grid_bits;
  uint _morton_size;
};
