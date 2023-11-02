/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#ifndef BOX_H_
#define BOX_H_

#include <glm/glm.hpp>
#include "ray.h"

using glm::vec3;

struct interval {
  float min;
  float max;
};

class Box {
 public:
  Box(vec3 min, vec3 max);
  bool intersect_bool(Ray ray);
 private:
  vec3 _min;
  vec3 _max;;
};

#endif  // BOX_H_
