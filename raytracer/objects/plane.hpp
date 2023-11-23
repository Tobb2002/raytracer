/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include "ray.hpp"
#include "object.hpp"

using glm::vec3;

class Plane : public Object{
 public:
  Plane();
  Plane(vec3 pos, vec3 normal, vec3 color);
  Plane(vec3 pos, vec3 normal, vec3 color1, vec3 color2);

  Intersection intersect(Ray ray) override;

  void print(void) override;

  vec3 get_color(vec3 point);

  // transformations

  void transform(mat4 transformation) override;


 private:
  vec3 _normal;
  vec3 _color;
  vec3 _color2;


  bool _two_colored;
};
