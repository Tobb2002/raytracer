/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "uniform_grid.hpp"

UniformGrid::UniformGrid(std::vector<Triangle> *triangles) {
  _data.triangles = triangles;
}

UniformGrid::UniformGrid(const UniformGrid &old) { _data = old._data; }
UniformGrid &UniformGrid::operator=(const UniformGrid &old) {
  _data = old._data;
  return *this;
}

void UniformGrid::build() {
  _data.morton.initialize_grid_size(_data.triangles, GRID_SIZE);

  for (uint triangle_id = 0; triangle_id < _data.triangles->size();
       triangle_id++) {
    add_to_grid(triangle_id);
  }
}

vec3 UniformGrid::compute_index(const vec3 &point) {
  vec3 size = _data.bounds.max - _data.bounds.min;
  return vec3((point.x - _data.bounds.min.x) / size.x,
              (point.y - _data.bounds.min.y) / size.y,
              (point.z - _data.bounds.min.z) / size.z);
}

std::vector<uint> *UniformGrid::get_ids(vec3 index) {
  // calculate morton index
  uint64_t morton_code = _data.morton.get_value(index);

  // return data at index
  return &_data.grid.at(morton_code);
}

void UniformGrid::add_id(vec3 index, uint id) {
  // calculate morton index
  uint64_t morton_code = _data.morton.get_value(index);
  // insert id
  if (_data.grid.contains(morton_code)) {
    _data.grid[morton_code].push_back(id);
  } else {  // new cell with given id
    _data.grid[morton_code] = {id};
  }
}

void UniformGrid::add_to_grid(uint triangle_id) {
  // get min, max index
  Triangle *triangle = _data.triangles->data() + triangle_id;
  vec3 index_min = compute_index(triangle->get_min_bounding());
  vec3 index_max = compute_index(triangle->get_max_bounding());

  // add to all cells in between
  for (uint x = index_min.x; x <= index_max.x; x++) {
    for (uint y = index_min.y; y <= index_max.y; y++) {
      for (uint z = index_min.z; z <= index_max.z; z++) {
        add_id(vec3(x, y, z), triangle_id);
      }
    }
  }
}

Intersection UniformGrid::intersect(const Ray &ray) {
  // check if box is intersected
  // find first intersecting cell -> convert ray origin to be in first cell

  // intersect cells recursively using DDA-Algorithm
  vec3 current_cell = get_first_cell(ray);

  // parameter t of the ray such that it intersects the x,y,z-bounds of the cell
  vec3 t = vec3(0);

  vec3 delta_t = get_delta_t();

  while (inside_grid(current_cell)) {
    if (intersect_cell(current_cell)) {
      return _best_intersection;
    }
    // update cell
    if (t_x < t_y && t_x < t_z) {
      // one step into x direction
    } else if () {
      // one step into y direction
    } else if () {
      // one step into z direction
    }
  }
}

Intersection UniformGrid::update_cell(vec3 *cell, const vec3 &delta_t) {
  // update cell index
  // update t_x, t_y, t_z
}
