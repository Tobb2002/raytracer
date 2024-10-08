/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "box.hpp"

#include <algorithm>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

float intersect_bounds(const bvh_box &box, const Ray &ray) {
  interval tx = {(box.min.x - ray.get_origin().x) / ray.get_direction().x,
                 (box.max.x - ray.get_origin().x) / ray.get_direction().x};

  interval ty = {(box.min.y - ray.get_origin().y) / ray.get_direction().y,
                 (box.max.y - ray.get_origin().y) / ray.get_direction().y};

  interval tz = {(box.min.z - ray.get_origin().z) / ray.get_direction().z,
                 (box.max.z - ray.get_origin().z) / ray.get_direction().z};

  // overlapping intervals indicate intersection
  // check box intersection

  if (tx.min > tx.max) {
    std::swap(tx.min, tx.max);
  }
  if (ty.min > ty.max) {
    std::swap(ty.min, ty.max);
  }
  if (tz.min > tz.max) {
    std::swap(tz.min, tz.max);
  }

  if (tx.max < 0 || ty.max < 0 || tz.max < 0) {
    return -1;
  }

  if (tx.min > ty.max || ty.min > tx.max) {
    // xy do not ovelap
    return -1;
  }
  if (tx.min > tz.max || tz.min > tx.max) {
    // xz do not overlap
    return -1;
  }
  if (ty.min > tz.max || tz.min > ty.max) {
    // yz do not overlap
    return -1;
  }

  // all intervalls overlap
  //
  return std::max(
      {std::max(tx.min, 0.f), std::max(ty.min, 0.f), std::max(tz.min, 0.f)});
}

Box::Box() {
  _min = vec3(FLT_MAX);
  _max = vec3(-FLT_MAX);
}
/**
 * @brief Construct a new Box:: Box object
 *
 * @param min vec3 defines min corner.
 * @param max vec3 defines max corner.
 */
Box::Box(vec3 min, vec3 max) { set_min_max(min, max); }

/**
 * @brief Set new position and size for the box.
 *
 * @param min vec3 defines min corner.
 * @param max vec3 defines max corner.
 */
void Box::set_min_max(vec3 min, vec3 max) {
  _min = min;
  _max = max;

  ensure_min_max();
}

/**
 * @brief Update values of min max if nessecary.
 *
 * @param min new possible min value.
 * @param max new possible min value.
 */
void Box::update_min_max(vec3 min, vec3 max) {
  for (int i = 0; i < 3; i++) {
    // update min
    if (min[i] < _min[i]) {
      _min[i] = min[i];
    }
    // update max
    if (max[i] > _max[i]) {
      _max[i] = max[i];
    }
  }
}

void Box::print(void) {
  std::cout << "------Box------\n";
  std::cout << "min: " << glm::to_string(_min) << "\n";
  std::cout << "max: " << glm::to_string(_max) << "\n";
  std::cout << "---------------\n";
}

/**
 * @brief Get middle point of the Box.
 *
 * @return vec3
 */
vec3 Box::get_middle(void) { return (_min + _max) * 0.5f; }

float Box::get_surface_area(void) {
  vec3 length = _max - _min;

  return 2 * length.x + 2 * length.y + 2 * length.z;
}

/**
 * @brief Ensure that all the min values are smaller than the max values.
 *
 */
void Box::ensure_min_max(void) {
  for (int i = 0; i < 3; i++) {
    if (_min[i] > _max[i]) {
      float tmp = _min[i];
      _min[i] = _max[i];
      _max[i] = tmp;
    }
  }
}

void update_min_bounding(vec3 *min, const vec3 &min_value) {
  for (int i = 0; i < 3; i++) {
    // update min
    if (min_value[i] < (*min)[i]) {
      (*min)[i] = min_value[i];
    }
  }
}

void update_max_bounding(vec3 *max, const vec3 &max_value) {
  for (int i = 0; i < 3; i++) {
    // update min
    if (max_value[i] > (*max)[i]) {
      (*max)[i] = max_value[i];
    }
  }
}

vec3 calculate_min(std::vector<Triangle> *triangles) {
  vec3 res_min = vec3(MAXFLOAT);

  for (Triangle t : *triangles) {
    update_min_bounding(&res_min, t.get_min_bounding());
  }
  return res_min;
}

vec3 calculate_max(std::vector<Triangle> *triangles) {
  vec3 res_max = vec3(-MAXFLOAT);
  for (Triangle t : *triangles) {
    update_max_bounding(&res_max, t.get_max_bounding());
  }
  return res_max;
}

bvh_box calculate_bounds(std::vector<Triangle> *triangles) {
  vec3 min = calculate_min(triangles);
  vec3 max = calculate_max(triangles);

  return bvh_box(min, max);
}

/**
 * @brief Check for intersection with ray.
 *
 * @param ray ray to check for intersection.
 * @return true if intersection found.
 * @return false else.
 */
bool Box::intersect_bool(Ray ray) {
  interval tx = {(_min.x - ray.get_origin().x) / ray.get_direction().x,
                 (_max.x - ray.get_origin().x) / ray.get_direction().x};

  interval ty = {(_min.y - ray.get_origin().y) / ray.get_direction().y,
                 (_max.y - ray.get_origin().y) / ray.get_direction().y};

  interval tz = {(_min.z - ray.get_origin().z) / ray.get_direction().z,
                 (_max.z - ray.get_origin().z) / ray.get_direction().z};

  // overlapping intervals indicate intersection
  // check box intersection

  if (tx.min > tx.max) {
    std::swap(tx.min, tx.max);
  }
  if (ty.min > ty.max) {
    std::swap(ty.min, ty.max);
  }
  if (tz.min > tz.max) {
    std::swap(tz.min, tz.max);
  }

  if (tx.min > ty.max || ty.min > tx.max) {
    // xy do not ovelap
    return false;
  }
  if (tx.min > tz.max || tz.min > tx.max) {
    // xz do not overlap
    return false;
  }
  if (ty.min > tz.max || tz.min > ty.max) {
    // yz do not overlap
    return false;
  }

  // all intervalls overlap
  return true;
}

void Box::apply_transform(mat4 transformation) {
  Object::apply_transform(transformation);

  _transform.transform_point(transformation, &_min);
  _transform.transform_point(transformation, &_max);

  ensure_min_max();
}
