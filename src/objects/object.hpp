/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <iostream>

#include "material.hpp"
#include "ray.hpp"
#include "transform.hpp"

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

/**
 * @brief Datastructure holding all relevant information of an intersection.
 */
struct Intersection {
  /// @brief true if Intersection found else false.
  bool found = false;
  /// @brief ray at position t gives intersection point.
  float t = MAXFLOAT;
  /// @brief Point of the intersection.
  vec3 point = vec3(0, 0, 0);
  /// @brief Normal of the intersecting surface.
  vec3 normal = vec3(0, 0, 0);
  /// @brief Material of the interscting surface.
  Material material;
};

/**
 * @brief Class to handle all transformable and intersecting Objects.
 */
class Object {
 public:
  Object();

  /***** Intersections *****/

  virtual Intersection intersect(const Ray& ray);
  virtual bool intersect_bool(const Ray& ray, float t_max);

  /***** Print DEBUG information */

  virtual void print(void);
  void print_matrices(void);

  /***** Transformations *****/

  void move(vec3 a);
  void rotate(vec3 axis, float degree);
  void rotate(vec3 point, vec3 axis, float degree);

  void transform(mat4 transformation);
  virtual void apply_transform(mat4 transformation);

  void update_view_transform(Transformation view_transform);

 protected:
  vec3 _origin;     // standart 0,0,0
  vec3 _direction;  // standart 0,0,1

  /***** Transformations *****/

  Transform _transform;
  Transformation _view_transform;
};
