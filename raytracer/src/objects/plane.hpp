/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include "ray.hpp"
#include "object.hpp"
#include "texture.hpp"

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
  Plane(vec3 pos,
        vec3 normal,
        Material material1,
        Material material2,
        vec2 size);
  Plane(vec3 pos, vec3 normal, Material material, vec2 size, std::string path_to_file);

  // settings
  void set_axis(bool enable);

  Intersection intersect(const Ray& ray) override;

  void print(void) override;

  Material get_material(vec3 point);

  // transformations

  void apply_transform(mat4 transformation) override;

  void calculate_direction(vec3 new_normal, vec3 old_direction);

  void calculate_normal(void);

 private:
  vec3 _normal;
  vec3 _point;
  vec2 _size;
  Material _material;
  Material _material2;
  Texture _texture;

  bool _two_colored;
  bool _textured;
  bool _axis_enable = false;
  bool _enable_size = false;
};
