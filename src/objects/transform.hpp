/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

// functions for transforming points
#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using glm::vec3, glm::vec4, glm::mat4;

struct Transformation {
  mat4 mat = glm::mat4(1.0);
  mat4 inv = glm::mat4(1.0);
};

struct Rotation {
  vec3 axis;
  float degree;
};

class Transform {
 public:
  mat4 add_translation(vec3 a);
  mat4 add_rotation(vec3 axis, float degree);
  mat4 add_rotation(vec3 point, vec3 axis, float degree);

  /***** Operations on matrices *****/
  mat4 add_rotation_mat(vec3 axis, float degree);

  void calculate_inverse_mat(void);
  void transform_point(mat4 transformation, vec3 *point);
  vec3 transform_point(mat4 t, vec3 point);

  mat4 get_combined(void);
  mat4 get_combined_inv(void);

  mat4 get_mat_rotation(void);

  /***** Calculations *****/

  Rotation calculate_rotation(vec3 dir1, vec3 dir2);

  vec3 origin_to_virtual(vec3 point);
  vec3 virtual_to_origin(vec3 point);

  void print(void);

 private:
  struct Matrices {
    mat4 translation = glm::mat4(1.f);
    mat4 rotation = glm::mat4(1.f);
    mat4 scale = glm::mat4(1.f);
  };

  vec3 get_current_origin(void);
  mat4 get_rotation_mat(Rotation rot);
  mat4 get_translation_mat(vec3 v);

  void apply_to_mat(mat4 *mat, mat4 t);

  Matrices _mat;
  Matrices _mat_inv;
};
