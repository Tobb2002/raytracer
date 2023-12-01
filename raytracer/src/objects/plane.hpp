/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include "ray.hpp"
#include "object.hpp"

using glm::vec3;


/**
 * @brief Object to render a plane.
 */
class Plane : public Object{
 public:
  Plane();
  void set_start_position(vec3 position, vec3 normal);
  Plane(vec3 pos, vec3 normal, Material material);
  Plane(vec3 pos, vec3 normal, Material material1, Material material2);

  Intersection intersect(Ray ray) override;

  void print(void) override;

  Material get_material(vec3 point);

  // transformations

  void apply_transform(mat4 transformation) override;

  void calculate_direction(vec3 new_normal, vec3 old_direction);

  void calculate_normal(void);

private:
  vec3 _normal;
  vec3 _point;
  Material _material;
  Material _material2;

  bool _two_colored;
};
