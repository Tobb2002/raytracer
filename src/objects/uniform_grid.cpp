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
  float t_0 = intersect_bounds();

  vec3 ray_origin = ray.get_point(t_0);
  vec3 ray_origin_grid = ray_origin - _data.bounds.min;
  vec3 ray_direction = ray.get_direction();

  // intersect cells recursively using DDA-Algorithm
  vec3 current_cell = get_cell(ray_origin_grid);

  // parameter t of the ray such that it intersects the next x,y,z-bounds of the
  // cell
  vec3 t_next = vec3(0);
  vec3 delta_t = vec3(0);

  // defines the step directions for every axis
  vec3 step;
  step.x = ray_direction.x > 0 ? 1 : -1;
  step.y = ray_direction.y > 0 ? 1 : -1;
  step.z = ray_direction.z > 0 ? 1 : -1;

  // calculate t_0  and delta_t
  if (ray_direction.x >= 0) {
    delta_t.x = _data.cell_size.x / ray_direction.x;
    t_next.x = ((current_cell.x + 1) * _data.cell_size.x - ray_origin_grid.x) /
               ray_direction.x;
  } else {
    delta_t.x = -_data.cell_size.x / ray_direction.x;
    t_next.x = (current_cell.x * _data.cell_size.x - ray_origin_grid.x) /
               ray_direction.x;
  }

  if (ray_direction.y >= 0) {
    delta_t.y = _data.cell_size.y / ray_direction.y;
    t_next.y = ((current_cell.y + 1) * _data.cell_size.y - ray_origin_grid.y) /
               ray_direction.y;
  } else {
    delta_t.y = -_data.cell_size.y / ray_direction.y;
    t_next.y = (current_cell.y * _data.cell_size.y - ray_origin_grid.y) /
               ray_direction.y;
  }

  if (ray_direction.z >= 0) {
    delta_t.z = _data.cell_size.z / ray_direction.z;
    t_next.z = ((current_cell.z + 1) * _data.cell_size.z - ray_origin_grid.z) /
               ray_direction.z;
  } else {
    delta_t.z = -_data.cell_size.z / ray_direction.z;
    t_next.z = (current_cell.z * _data.cell_size.z - ray_origin_grid.z) /
               ray_direction.z;
  }

  // iterating trough the cells
  // ray parrameter of the current intersection
  float t = 0;

  while (inside_grid(current_cell)) {
    if (intersect_cell(current_cell)) {
      return _best_intersection;
    }

    // update cell -> one step into direection of smallest t_next
    if (t_next.x <= t_next.y && t_next.x <= t_next.z) {
      t = t_next.x;
      t_next.x += delta_t.x;
      current_cell.x += step.x;
    } else if (t_next.y <= t_next.x && t_next.y <= t_next.z) {
      t = t_next.y;
      t_next.y += delta_t.y;
      current_cell.y += step.y;
    } else if (t_next.z <= t_next.x && t_next.z <= t_next.y) {
      t = t_next.z;
      t_next.z += delta_t.z;
      current_cell.z += step.z;
    }
  }
}

vec3 UniformGrid::get_cell(vec3 point) {
  // point relative to grid origin
  vec3 point_grid = point - _data.bounds.min;

  return glm::floor(point_grid / _data.cell_size);
}

bool UniformGrid::inside_grid(vec3 index) {
  for (size_t a = 0; a < 3; a++) {
    if (index[a] < 0 || index[a] > _data.resolution[a]) {
      return false;
    }
  }

  return true;
}
