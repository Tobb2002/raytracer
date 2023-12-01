/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

// functions for transforming points
#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using glm::vec3, glm::vec4, glm::mat4;

struct Transformation{
  mat4 mat = glm::mat4(1.0);
  mat4 inv = glm::mat4(1.0);
};

class Transform {
 public:

  mat4 add_translation(vec3 a);
  mat4 add_rotation(vec3 axis, float degree);

  /***** Operations on matrices *****/
  mat4 add_rotation_mat(vec3 axis, float degree);

  void calculate_inverse_mat(void);
  void transform_point(mat4 transformation, vec3 *point);

  mat4 get_combined(void);
  mat4 get_combined_inv(void);

  vec3 origin_to_virtual(vec3 point);
  vec3 virtual_to_origin(vec3 point);

  void print(void);

 private:
  struct Matrices {
    mat4 translation = glm::mat4(1.f);
    mat4 rotation = glm::mat4(1.f);
    mat4 scale = glm::mat4(1.f);
  };
  
  Matrices _mat;
  Matrices _mat_inv;
};