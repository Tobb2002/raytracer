/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include "ray.hpp"
#include "object.hpp"

using glm::vec3;


/**
 * @brief Object to render a sphere.
 */
class Sphere : public Object{
 public:
  Sphere();
  Sphere(vec3 pos, float radius);
  Sphere(vec3 pos, float radius, Material material);
  void set_start_position(vec3 position, float radius);

  Intersection intersect(Ray ray) override;

  void print(void) override;

  Material get_material(vec3 point);

  // transformations

  //void apply_transform(mat4 transformation) override;

  vec3 calculate_normal(vec3 surface_point);

private:
  vec3 _normal;
  float _radius;
  Material _material;
};
