/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <sys/types.h>

#include <glm/glm.hpp>
#include <map>
#include <vector>

#include "bvh_tree.hpp"
#include "morton.hpp"
#include "triangle.hpp"

#define GRID_SIZE 100

/// @brief struct to store data needed by the uniform grid.
struct grid_data {
  std::vector<Triangle>* triangles;

  /// @brief outer bounding box of object.
  bvh_box bounds;

  /// @brief number of grid cells per axis.
  vec3 resolution = vec3(GRID_SIZE);

  /// @brief stores triangle ids in cells using hash_map index is calculated
  /// with morton codes
  std::map<uint64_t, std::vector<uint>> grid;

  /// @brief class to calculate morton indices.
  Morton morton;
};

struct grid_properties {
  bvh_box bounds;
  vec3 resolution;
  vec3 cell_size;
};

// alternative: grid cells in array indexed by morton codes.
// -> calculate grid cell using morton codes
// -> put triangles into grid cells morton code min + max all cells in between
// -> traversing check cells using morton codes
// nein könnten mehr zellen gespeichert werden:
// every grid cell saves maximimum and minimum index of sorted triangle_ids

struct grid_index {
  uint x;
  uint y;
  uint z;
};

class UniformGrid {
 public:
  explicit UniformGrid(std::vector<Triangle>* triangles);
  UniformGrid(const UniformGrid& old);
  UniformGrid& operator=(const UniformGrid& old);

  void build();

  /**
   * @brief Return best triangle intersection if found.
   *
   * @param ray
   * @return Intersection
   */
  Intersection intersect(const Ray& ray);

 private:
  // edit grid
  void add_id(vec3 index, uint id);

  void add_to_grid(uint triangle_id);

  std::vector<uint>* get_ids(vec3 index);

  /// @brief compute the grid index(x,y,z) of a given point in space.
  vec3 compute_index(const vec3& point);
  grid_data _data;
};
