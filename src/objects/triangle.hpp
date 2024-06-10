/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>

#include "material.hpp"
#include "object.hpp"
#include "ray.hpp"

class Triangle : public Object {
 public:
  Triangle();
  Triangle(vec3 points[3], Material material);
  Triangle(vec3 points[3], Material material, vec2 uv_coordinates[3]);

  Intersection intersect(const Ray& ray) override;

  // transformations
  void apply_transform(mat4 transformation) override;

  // setters
  void set_vertex_normals(vec3 normals[3]);

  // getters
  vec3 get_normal();
  vec3 get_pos();
  Material get_material(void);
  void set_material(Material material);

  vec3 get_min_bounding(void);
  vec3 get_max_bounding(void);

  void print() override;

 private:
  vec3 _p[3];
  // vertex texture coordinates _p_uv -1 means no texture loaded
  vec2 _p_uv[3];
  vec3 _middle_point;
  Material _material;
  vec3 _normal;
  // vertex normals
  vec3 _p_normal[3];
  bool _enable_smooth_normals = false;

  vec3 calculate_normal(void);
  vec3 calculate_normal_interpolated(vec3 uvw);
  vec3 calculate_middle(void);
};
