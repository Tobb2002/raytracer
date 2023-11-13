/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <glm/glm.hpp>
#include "ray.h"
#include "object.hpp"

using glm::vec3;

class Triangle {
 public:
  Triangle();
  Triangle(vec3 points[3], vec3 color);

  float intersect(Ray ray);
  bool intersect_bool(Ray ray);

  //transformations
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

#endif  // TRIANGLE_H_
