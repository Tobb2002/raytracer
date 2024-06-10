/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>

#include "object.hpp"
#include "ray.hpp"
#include "texture.hpp"

using glm::vec3;

/**
 * @brief Object to render a sphere.
 */
class Sphere : public Object {
 public:
  Sphere();
  Sphere(vec3 pos, float radius);
  Sphere(vec3 pos, float radius, Material material);
  Sphere(vec3 pos, float radius, Material material, std::string path_to_file);

  void set_start_position(vec3 position, float radius);
  void enable_texture(bool enable);

  Intersection intersect(const Ray& ray) override;

  void print(void) override;

  Material get_material(vec3 point);

  // transformations

  void apply_transform(mat4 transformation) override;
  vec3 get_direction();

  vec3 calculate_normal(vec3 surface_point);

 private:
  vec3 _normal;
  float _radius;
  vec3 _direction_point;
  Material _material;
  Texture _texture;
  bool _enable_texture;
};
