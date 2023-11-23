/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include "ray.hpp"
#include "object.hpp"

using glm::vec3;

struct interval {
  float min;
  float max;
};

class Box : public Object{
 public:
  Box();
  Box(vec3 min, vec3 max);
  void set_min_max(vec3 min, vec3 max);
  void update_min_max(vec3 min, vec3 max);
  bool intersect_bool(Ray ray);

  vec3 get_middle(void);

  // transformations
  void move(vec3 vec);
  void transform(mat4 transformation) override;

  void print(void);

  // function ensures that min is smaller than max for x, y, z
  void ensure_min_max(void);
 private:
  vec3 _min;
  vec3 _max;;
};
