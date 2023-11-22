/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include "ray.hpp"
#include "object.hpp"

using glm::vec3;

class Triangle : public Object{
 public:
  Triangle();
  Triangle(vec3 points[3], vec3 color);

  Intersection intersect(Ray ray) override;

  // transformations
  void move(vec3 vec);

  // getters
  vec3 get_color();
  vec3 get_normal();
  void set_color(vec3 color);

  void print();
 private:
  vec3 _p[3];
  vec3 _color;
  vec3 _normal;

  vec3 calculate_normal(void);
};
