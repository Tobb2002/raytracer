/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "./ray.h"
#include <iostream>

// constructor
Ray::Ray(vec3 origin, vec3 direction) {
  Ray::origin = origin;
  Ray::direction = direction;
}

vec3 Ray::get_point(float t) {
  return origin + (t * direction);
}
