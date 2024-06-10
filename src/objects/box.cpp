/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "box.hpp"

#include <glm/gtx/string_cast.hpp>
#include <iostream>

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
