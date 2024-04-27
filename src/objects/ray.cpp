/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "ray.hpp"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

// constructor
Ray::Ray(vec3 origin, vec3 direction) {
  Ray::_origin = origin;
  Ray::_direction = glm::normalize(direction);
}

// getter
vec3 Ray::get_origin() const { return _origin; }
vec3 Ray::get_direction() const { return _direction; }

vec3 Ray::get_point(float t) const {
  return _origin + (t * _direction);
}

void Ray::move_into_dir(float x) {
  _origin += _direction * x;
}

void Ray::print(void) {
  std::cout << "------Ray------- " <<  std::endl;
  std::cout << "origin: " << glm::to_string(_origin) << std::endl;
  std::cout << "direction: " << glm::to_string(_direction) << std::endl;
  std::cout << "---------------- " <<  std::endl;
}
