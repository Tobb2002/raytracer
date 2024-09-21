/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>

#include "material.hpp"
#include "object.hpp"
#include "ray.hpp"

struct TriangleIntersection {
  /// @brief true if Intersection found else false.
  bool found = false;
  /// @brief ray at position t gives intersection point.
  float t = MAXFLOAT;
  /// @brief Point of the intersection.
  vec3 point = vec3(0, 0, 0);
  /// @brief Normal of the intersecting surface.
  vec3 normal = vec3(0, 0, 0);
  /// @brief Material of the interscting surface.
  uint material_id;
  /// @brief texture coordinates if available.
  vec2 texture_uv = vec2(-1, -1);
  /// @brief coordinates if available.

  // vec2 normal_uv = vec2(-1,-1);
};

class Triangle : public Object {
 public:
  Triangle();
  Triangle(vec3 points[3], uint material_id);
  Triangle(vec3 points[3], uint material_id, vec2 uv_coordinates[3]);

  TriangleIntersection intersect_triangle(const Ray& ray);

  // transformations
  void apply_transform(mat4 transformation) override;

  // setters
  void set_vertex_normals(vec3 normals[3]);
  void set_vertex_texture(vec2 texture_uv[3]);

  // getters
  vec3 get_normal();
  vec3 get_pos();
  uint get_material(void);
  void set_material(uint material_id);

  vec3 get_min_bounding(void);
  vec3 get_max_bounding(void);

  void print() override;

 private:
  vec3 _p[3];
  // vertex texture coordinates _p_uv -1 means no texture loaded
  vec2 _p_uv[3];
  vec3 _middle_point;
  // Material _material;
  uint _material_id = 0;
  vec3 _normal;
  // vertex normals
  vec3 _p_normal[3];
  bool _enable_smooth_normals = false;

  vec3 calculate_normal(void);
  vec3 calculate_normal_interpolated(vec3 uvw);
  vec2 calculate_texture_interpolated(vec3 uvw);
  vec3 calculate_middle(void);
};
