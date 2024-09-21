/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "uniform_grid.hpp"

#include "box.hpp"

UniformGrid::UniformGrid(std::vector<Triangle> *triangles) {
  _data.triangles = triangles;
}

UniformGrid::UniformGrid(const UniformGrid &old) { _data = old._data; }
UniformGrid &UniformGrid::operator=(const UniformGrid &old) {
  _data = old._data;
  return *this;
}

void UniformGrid::build(std::vector<Triangle> *triangles) {
  _data.triangles = triangles;
  _data.morton.initialize_grid_size(_data.triangles, GRID_SIZE);

  // set properties
  _data.resolution = vec3(GRID_SIZE);

  // calculate bounds
  // TODO(tobi) calculate bounds in the right way
  _data.bounds = calculate_bounds(triangles);
  // check if cell size 0 can appear
  for (size_t a = 0; a < 3; a++) {
    if (_data.bounds.min[a] == _data.bounds.max[a]) {
      _data.bounds.max[a] += 0.01;
    }
  }

  _data.cell_size = (_data.bounds.max - _data.bounds.min) / _data.resolution;

  for (uint triangle_id = 0; triangle_id < _data.triangles->size();
       triangle_id++) {
    add_to_grid(triangle_id);
  }
  std::cout << "filled cells: " << _data.grid.size() << "\n";
}

vec3 UniformGrid::compute_index(const vec3 &point) {
  return glm::floor((point - _data.bounds.min) / _data.cell_size);
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
  _data.grid[morton_code].push_back(id);
}

void UniformGrid::add_to_grid(uint triangle_id) {
  // get min, max index
  Triangle *triangle = _data.triangles->data() + triangle_id;
  vec3 index_min = get_cell(triangle->get_min_bounding());
  vec3 index_max = get_cell(triangle->get_max_bounding());

  // add to all cells in between
  for (uint x = index_min.x; x <= index_max.x; x++) {
    for (uint y = index_min.y; y <= index_max.y; y++) {
      for (uint z = index_min.z; z <= index_max.z; z++) {
        add_id(vec3(x, y, z), triangle_id);
      }
    }
  }
}

void UniformGrid::set_triangles(std::vector<Triangle> *triangles) {
  _data.triangles = triangles;
}

TriangleIntersection UniformGrid::intersect(const Ray &ray) {
  _best_intersection = TriangleIntersection();
  // check if box is intersected
  // find first intersecting cell -> convert ray origin to be in first cell
  float t_0 = intersect_bounds(_data.bounds, ray);

  if (t_0 < 0) {
    // ray does not intersect
    return _best_intersection;
  }

  vec3 ray_origin = ray.get_point(t_0);
  vec3 ray_origin_grid = ray_origin - _data.bounds.min;
  vec3 ray_direction = ray.get_direction();

  // intersect cells recursively using DDA-Algorithm
  vec3 current_cell = get_cell(ray_origin);

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

  // std::cout << "start\n";
  while (inside_grid(current_cell)) {
    // std::cout << "checkt cell: " << current_cell.x << ","<< current_cell.y <<
    // ","<< current_cell.z << "\n";
    if (intersect_cell(current_cell, ray)) {
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

  _best_intersection = TriangleIntersection();
  return _best_intersection;
}

vec3 UniformGrid::get_cell(vec3 point) {
  // point relative to grid origin
  vec3 point_grid = point - _data.bounds.min;

  return glm::floor(point_grid / _data.cell_size);
}

bool UniformGrid::inside_grid(vec3 index) {
  for (size_t a = 0; a < 3; a++) {
    if (index[a] < 0 || index[a] > _data.resolution[a] + 1) {
      return false;
    }
  }
  return true;
}

bool UniformGrid::intersect_cell(vec3 index, const Ray &ray) {
  // std::cout << "intersect cell\n";
  uint64_t morton_code = _data.morton.get_value(index);

  if (!_data.grid.contains(morton_code)) {
    _best_intersection = TriangleIntersection();
    return false;
  }
  std::vector<uint> *triangle_ids = get_ids(index);

  uint best_triangle_id = 0;
  float t_min = MAXFLOAT;

  for (uint i : *triangle_ids) {
    TriangleIntersection t_i = _data.triangles->at(i).intersect_triangle(ray);

    if (t_i.found && t_i.t < t_min) {
      t_min = t_i.t;
      best_triangle_id = i;
    }
  }

  TriangleIntersection res =
      (_data.triangles->data() + best_triangle_id)->intersect_triangle(ray);

  update_intersection(&_best_intersection, res);

  return _best_intersection.found;
}

bool UniformGrid::update_intersection(
    TriangleIntersection *intersect,
    const TriangleIntersection &new_intersect) {
  if (new_intersect.found) {
    // update intersection if the new one is closer
    if (new_intersect.t < intersect->t) {
      *intersect = new_intersect;
      return true;
    }
  }
  return false;
}
