/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <iostream>
#include <glm/glm.hpp>

#include "ray.hpp"
#include "material.hpp"

using glm::vec3;
using glm::vec4;
using glm::mat4;

/**
 * @brief Datastructure holding all relevant information of an intersection.
 */
struct Intersection {
  /// @brief true if Intersection found else false.
  bool found;
  /// @brief ray at position t gives intersection point.
  float t;
  /// @brief Point of the intersection.
  vec3 point;
  /// @brief Normal of the intersecting surface.
  vec3 normal;
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

  virtual Intersection intersect(Ray ray);
  virtual bool intersect_bool(Ray ray, float t_max);

  /***** Print DEBUG information */

  virtual void print(void);
  void print_matrices(void);

  /***** Transformations *****/

  virtual void move(vec3 a);
  virtual void rotate(vec3 axis, float degree);
  virtual void transform(mat4 transformation);

  vec3 origin_to_virtual(vec3 point);
  vec3 virtual_to_origin(vec3 point);

 protected:
  vec3 _origin;  // standart 0,0,0
  vec3 _direction;  // standart 0,0,1

  /***** Transformations *****/

  mat4 _mat_translation;
  mat4 _mat_rotation;
  mat4 _mat_scale;

  mat4 _mat_inv_translation;
  mat4 _mat_inv_rotation;
  mat4 _mat_inv_scale;

  void calculate_inverse_mat(void);
  void transform_matrices(mat4 transformation);
  void transform_point(mat4 transformation, vec3 *point);

  void calculate_direction(vec3 direction);

 private:
  void initialize_matrices(void);
};
