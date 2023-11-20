/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <iostream>
#include <glm/glm.hpp>

#include "ray.hpp"

using glm::vec3;
using glm::mat4;

struct Intersection {
  bool found;
  float t;
  vec3 point;
  vec3 normal;
  vec3 color;
};

class Object {
 public:
  virtual Intersection intersect(Ray ray);
  virtual bool intersect_bool(Ray ray, float t_max);

  virtual void print(void);

  // transformation functions
  virtual void rotate(void);

 private:
  vec3 _origin;
  vec3 _color;

  // transformation
  mat4 _mat_translation;
  mat4 _mat_rotation;
  mat4 _mat_scale;
};
