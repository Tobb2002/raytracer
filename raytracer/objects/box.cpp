/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "box.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>


Box::Box() {
  _min = vec3(0, 0, 0);
  _max = vec3(0, 0, 0);
}
Box::Box(vec3 min, vec3 max) {
  set_min_max(min, max);
}

void Box::set_min_max(vec3 min, vec3 max) {
  _min = min;
  _max = max;

  ensure_min_max();
}

void Box::print(void) {
  std::cout << "------Box------\n";
  std::cout << "min: " << glm::to_string(_min) << "\n";
  std::cout << "max: " << glm::to_string(_max) << "\n";
  std::cout << "---------------\n";
}

void Box::ensure_min_max(void) {
  // ensure that all values in min are smaller than the values in max
  if (_min.x > _max.x) {
    float tmp = _min.x;
    _min.x = _max.x;
    _max.x = tmp;
  }
  if (_min.y > _max.y) {
    float tmp = _min.y;
    _min.y = _max.y;
    _max.y = tmp;
  }
  if (_min.z > _max.z) {
    float tmp = _min.z;
    _min.z = _max.z;
    _max.z = tmp;
  }
}

// checks if a ray intersects a box
bool Box::intersect_bool(Ray ray) {
  interval tx = {(_min.x - ray.get_origin().x) / ray.get_direction().x,
                (_max.x - ray.get_origin().x) / ray.get_direction().x};
  
  interval ty = {(_min.y - ray.get_origin().y) / ray.get_direction().y,
                (_max.y - ray.get_origin().y) / ray.get_direction().y};

  interval tz = {(_min.z - ray.get_origin().z) / ray.get_direction().z,
                (_max.z - ray.get_origin().z) / ray.get_direction().z};

  // overlapping intervals indicate intersection
  // check box intersection

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