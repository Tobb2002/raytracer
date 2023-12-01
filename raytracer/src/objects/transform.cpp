/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "transform.hpp"
#include <glm/gtx/string_cast.hpp>
#include <iostream>

mat4 Transform::add_translation(vec3 a) {
  mat4 t = glm::mat4x4(vec4(1, 0, 0, 0),
                vec4(0, 1, 0, 0),
                vec4(0, 0, 1, 0),
                vec4(a, 1));

  _mat.translation = _mat.translation * t;
  calculate_inverse_mat();

  return t;
}

/**
 * @brief adds_rotation around object origin.
 * 
 * @param axis 
 * @param degree 
 * @return mat4 
 */
mat4 Transform::add_rotation(vec3 axis, float degree) {
  axis = glm::normalize(axis);  // recommended for glm library
  mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(degree), axis);

  // move to origin -> rotate -> move back
  mat4 t = _mat.translation * rot * _mat_inv.translation;

  // update rotation matrices
  _mat.rotation = rot * _mat.rotation;
  calculate_inverse_mat();

  return t;
}

/**
 * @brief add a rotation only to rotation matrix.
 * 
 * @param axis 
 * @param degree 
 */
mat4 Transform::add_rotation_mat(vec3 axis, float degree) {
  axis = glm::normalize(axis);  // recommended for glm library
  mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(degree), axis);

  // update rotation matrices
  _mat.rotation = rot * _mat.rotation;
  calculate_inverse_mat();

  return _mat.rotation;
}

void Transform::calculate_inverse_mat(void)
{
  // translation T^-1(t) = T(-t)
  _mat_inv.translation =  mat4(vec4(1, 0, 0, 0),
                               vec4(0, 1, 0, 0),
                               vec4(0, 0, 1, 0),
                               vec4(_mat.translation[3][0] * -1,
                                    _mat.translation[3][1] * -1,
                                    _mat.translation[3][2] * -1,
                                    1));
  // rotation R^-1 = transpose(Rx)
  _mat_inv.rotation = glm::transpose(_mat.rotation);
  // scale S^-1 = S(1/s)
  // _mat_inv_scale = glm::compScale
}

/**
 * @brief Transform a point.
 * 
 * @param transformation transformation matrix.
 * @param point Point to be transformed.
 */
void Transform::transform_point(mat4 transformation, vec3 *point) {
  *point =  transformation * vec4(*point, 1);
}

mat4 Transform::get_combined(void) {
  return _mat.translation * _mat.rotation;
}

mat4 Transform::get_combined_inv(void) {
  return _mat_inv.rotation * _mat.translation;
}

vec3 Transform::origin_to_virtual(vec3 point) {
  mat4 transform = _mat.translation * _mat.rotation;
  vec3 res = point;
  transform_point(transform, &res);
  return res;
}

vec3 Transform::virtual_to_origin(vec3 point) {
  mat4 transform = _mat_inv.rotation * _mat_inv.translation;
  vec3 res = point;
  transform_point(transform, &res);
  return res;
}

void Transform::print() {
  std::cout << "---Transform----\n";
  std::cout << "translation:\n" << glm::to_string(_mat.translation) << "\n";
  std::cout << "rotation:\n" << glm::to_string(_mat.rotation) << "\n";
  std::cout << "inv translation:\n" << glm::to_string(_mat_inv.translation) <<
      "\n";
  std::cout << "inv rotation:\n" << glm::to_string(_mat_inv.rotation) << "\n";
  std::cout << "----------------\n";
}