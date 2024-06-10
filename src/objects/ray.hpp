/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

using glm::vec3;

class Ray {
 public:
  Ray(vec3 origin, vec3 direction);
  // getter
  vec3 get_origin() const;
  vec3 get_direction() const;
  vec3 get_point(float t) const;

  // functions
  void move_into_dir(float x);
  void print(void);

 private:
  vec3 _origin;
  vec3 _direction;
};
