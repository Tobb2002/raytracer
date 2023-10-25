/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "./ray.h"
#include <iostream>

// constructor
Ray::Ray(vec3 origin, vec3 direction) {
  Ray::_origin = origin;
  Ray::_direction = direction;
}

// getter
vec3 Ray::get_origin() { return _origin; }
vec3 Ray::get_direction() { return _direction; }

vec3 Ray::get_point(float t) {
  return _origin + (t * _direction);
}
