/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#ifndef PLANE_H_
#define PLANE_H_

#include <glm/glm.hpp>
#include "ray.h"
#include "object.hpp"

using glm::vec3;

class Plane : public Object{
 public:
  Plane();
  Plane(vec3 pos, vec3 normal, vec3 color);

  Intersection intersect(Ray ray) override;
  bool intersect_bool(Ray ray) override;

  //transformations
  void move(vec3 vec);

  // getters
  vec3 get_color();
  vec3 get_normal();
  void set_color(vec3 color);

 private:
  vec3 _position;
  vec3 _normal;
  vec3 _color;
};

#endif  // PLANE_H_