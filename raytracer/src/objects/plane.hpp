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
  Plane(vec3 pos, vec3 normal, Material material);
  Plane(vec3 pos, vec3 normal, Material material1, Material material2);

  Intersection intersect(Ray ray) override;

  void print(void) override;

  Material get_material(vec3 point);

  // transformations

  void transform(mat4 transformation) override;


 private:
  vec3 _normal;
  Material _material;
  Material _material2;


  bool _two_colored;
};
