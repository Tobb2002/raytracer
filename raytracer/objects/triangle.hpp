/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include "ray.hpp"
#include "object.hpp"
#include "material.hpp"

using glm::vec3;

class Triangle : public Object{
 public:
  Triangle();
  Triangle(vec3 points[3], Material material);

  Intersection intersect(Ray ray) override;

  // transformations
  void move(vec3 vec) override;
  void transform(mat4 transformation) override;

  // getters
  vec3 get_normal();
  Material get_material(void);
  void set_material(Material material);

  vec3 get_min_bounding(void);
  vec3 get_max_bounding(void);

  void print();
 private:
  vec3 _p[3];
  Material _material;
  vec3 _normal;

  vec3 calculate_normal(void);
};
