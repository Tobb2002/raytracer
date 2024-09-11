/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <sys/types.h>

#include <glm/glm.hpp>
#include <map>
#include <unordered_map>
#include <vector>

#include "bvh_tree.hpp"
#include "morton.hpp"
#include "triangle.hpp"

// number of cells per axis = grid size + 1
#define GRID_SIZE 100

/// @brief struct to store data needed by the uniform grid.
struct grid_data {
  std::vector<Triangle>* triangles;

  /// @brief outer bounding box of object.
  bvh_box bounds;

  /// @brief number of grid cells per axis.
  vec3 resolution = vec3(GRID_SIZE);

  vec3 cell_size;

  /// @brief stores triangle ids in cells using hash_map index is calculated
  /// with morton codes
  std::unordered_map<uint64_t, std::vector<uint>> grid;

  /// @brief class to calculate morton indices.
  Morton morton;
};

// alternative: grid cells in array indexed by morton codes.
// -> calculate grid cell using morton codes
// -> put triangles into grid cells morton code min + max all cells in between
// -> traversing check cells using morton codes
// neinkönnten mehr zellen gespeichert werden:
// every grid cell saves maximimum and minimum index of sorted triangle_ids

struct grid_index {
  uint x;
  uint y;
  uint z;
};

class UniformGrid {
 public:
  UniformGrid() {}
  explicit UniformGrid(std::vector<Triangle>* triangles);
  UniformGrid(const UniformGrid& old);
  UniformGrid& operator=(const UniformGrid& old);

  void build(std::vector<Triangle>* triangles);

  /**
   * @brief Return best triangle intersection if found.
   *
   * @param ray
   * @return Intersection
   */
  Intersection intersect(const Ray& ray);

  void set_triangles(std::vector<Triangle>* triangles);

 private:
  // edit grid
  void add_id(vec3 index, uint id);

  void add_to_grid(uint triangle_id);

  std::vector<uint>* get_ids(vec3 index);

  /// @brief compute the grid index(x,y,z) of a given point in space.
  vec3 compute_index(const vec3& point);

  // --------------------------------------------------------------------------
  // functions for intersecting the grid

  /// @brief calculates the cell corrosponding to a point in space.
  vec3 get_cell(vec3 point);

  /// @brief intersect cell best intersection is saved in _best_intersection
  bool intersect_cell(vec3 index, const Ray& ray);

  bool update_intersection(Intersection* intersect,
                           const Intersection& new_intersect);

  Intersection _best_intersection;

  /// @brief checks if given cell index is inside the grid.
  bool inside_grid(vec3 index);

  grid_data _data;
};
