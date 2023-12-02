/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "transform.hpp"
#include <glm/gtx/string_cast.hpp>
#include <iostream>

mat4 Transform::add_translation(vec3 a) {
  mat4 t = get_translation_mat(a);
  _mat.translation = _mat.translation * t;
  calculate_inverse_mat();

  return t;
}

/**
 * @brief adds_rotation around object origin.
 * 
 * @param axis rotation axis.
 * @param degree degree to rotate.
 * @return mat4 
 */
mat4 Transform::add_rotation(vec3 axis, float degree) {
  axis = glm::normalize(axis);  // recommended for glm library
  mat4 rot = get_rotation_mat({axis, degree});

  // move to origin -> rotate -> move back
  mat4 t = _mat.translation * rot * _mat_inv.translation;

  // update rotation matrices
  _mat.rotation = rot * _mat.rotation;
  calculate_inverse_mat();

  return t;
}

/**
 * @brief Add rotation around a point.
 * 
 * @param point point to rotate around.
 * @param axis Rotation axis
 * @param degree degree to rotate.
 * @return mat4 
 */
mat4 Transform::add_rotation(vec3 point, vec3 axis, float degree) {
  vec3 origin = get_current_origin();
  vec3 v = origin - point;
  
  //calculate rotation
  mat4 rot = get_rotation_mat({axis, degree});
  transform_point(rot, &v);

  // calculate translation from world origin
  mat4 origin_to_pos =
      get_translation_mat(v) * get_translation_mat(point);

  // update matrices
  apply_to_mat(&_mat.rotation, rot);
  apply_to_mat(&_mat.translation, origin_to_pos * _mat_inv.translation);
  // calculate transformation to move object to new position

  mat4 t = origin_to_pos * rot * _mat_inv.translation;

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
  mat4 rot = get_rotation_mat({axis, degree});

  // update rotation matrices
  apply_to_mat(&_mat.rotation, rot);
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

vec3 Transform::transform_point(mat4 t, vec3 point) {
  return t * vec4(point, 1);
}

/**
 * @brief Get the combined matrix of current transformation.
 * 
 * @return mat4 
 */
mat4 Transform::get_combined(void) {
  return _mat.translation * _mat.rotation;
}

/**
 * @brief Get the combined inverse matrix of current transformation.
 * 
 * @return mat4 
 */
mat4 Transform::get_combined_inv(void) {
  return _mat_inv.rotation * _mat.translation;
}

/// @brief Calculate th angle between two directions
Rotation Transform::calculate_rotation(vec3 dir1, vec3 dir2) {
  vec3 axis = glm::cross(dir2, dir1);

  // if new_dir points to exactly th oposite direction axis = 0, 0, 0
  // TODO(tobi) check if this is right or nonsense
  if (axis.x == 0 && axis.y == 0 && axis.z == 0) {
    axis = vec3(0, 1, 0);
  }

  float radian = glm::acos(glm::dot(dir1, dir2) /
      glm::length(dir1) * glm::length(dir2));


  Rotation rot = {axis, glm::degrees(radian)};
  
  return rot;
}

vec3 Transform::origin_to_virtual(vec3 point)
{
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

vec3 Transform::get_current_origin(void) {
  return transform_point(_mat.translation, vec3(0, 0, 0));
}

mat4 Transform::get_rotation_mat(Rotation rot) {
  return glm::rotate(glm::mat4(1.0), glm::radians(rot.degree), rot.axis);
}

mat4 Transform::get_translation_mat(vec3 v) {
  return glm::mat4x4(vec4(1, 0, 0, 0),
                vec4(0, 1, 0, 0),
                vec4(0, 0, 1, 0),
                vec4(v, 1));
}

void Transform::apply_to_mat(mat4 *mat, mat4 t) {
  _mat.rotation = t * *mat;
}
