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
  Triangle(vec3 points[3], Material material, vec2 uv_coordinates[3]);

  Intersection intersect(Ray ray) override;
  Intersection intersect(Ray *ray);

  // transformations
  void apply_transform(mat4 transformation) override;

  // getters
  vec3 get_normal();
  vec3 get_pos();
  Material get_material(void);
  void set_material(Material material);

  vec3 get_min_bounding(void);
  vec3 get_max_bounding(void);

  void print();

 private:
  vec3 _p[3];
  // _p_uv -1 means no texture loaded
  vec2 _p_uv[3];
  vec3 _middle_point;
  Material _material;
  vec3 _normal;

  vec3 calculate_normal(void);
  vec3 calculate_middle(void);
};
