/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "object.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/component_wise.hpp>

using glm::vec4;

Object::Object() {
  _origin_start = vec3(0, 0, 0);
  _direction_start = vec3(0, 0, 1);
  _origin = vec3(0, 0, 0);
  _direction = vec3(0, 0, 1);
  // initialize transformation matrices
  initialize_matrices();
  calculate_inverse_mat();
}

Intersection Object::intersect(Ray ray) {
  Material material;
  return {false, 0, vec3(0, 0, 0), vec3(0, 0, 0), material};
}
bool Object::intersect_bool(Ray ray, float t_max) {
  Intersection i = intersect(ray);

  if (i.found && i.t < t_max) {
    return true;
  }

  return false;
}

void Object::print(void) {
  std::cout << "-----Object-----\n";
}

void Object::print_matrices(void) {
  std::cout << "-----Object-----\n";
  std::cout << "-----Matrix-----\n";
  std::cout << "translation:\n" << glm::to_string(_mat_translation) << "\n";
  std::cout << "rotation:\n" << glm::to_string(_mat_rotation) << "\n";
  std::cout << "inv translation:\n" << glm::to_string(_mat_inv_translation) << "\n";
  std::cout << "inv rotation:\n" << glm::to_string(_mat_inv_rotation) << "\n";
  std::cout << "----------------\n";
}

/*
 * Transformation
 */

void Object::initialize_matrices(void) {
  // matrices should be identity matrix at beginning.
  _mat_translation = glm::mat4(1.0);
  _mat_rotation = glm::mat4(1.0);
  _mat_scale = glm::mat4(1.0);

  calculate_inverse_mat();
}


void Object::calculate_inverse_mat(void) {
  // translation T^-1(t) = T(-t)
  _mat_inv_translation =  mat4(vec4(1, 0, 0,0),
                               vec4(0, 1, 0,0),
                               vec4(0, 0, 1,0),
                               vec4(_mat_translation[3][0] * -1,
                                    _mat_translation[3][1] * -1,
                                    _mat_translation[3][2] * -1,
                                    1));
  // rotation R^-1 = transpose(Rx)
  _mat_inv_rotation = glm::transpose(_mat_rotation);
  // scale S^-1 = S(1/s)
  //_mat_inv_scale = glm::compScale
}

/*
 * Moves the object and all it's components along the vector a.
 */
void Object::move(vec3 a) {
  std::cout << "moving object\n";
  mat4 t = glm::mat4x4(vec4(1, 0, 0, 0),
                vec4(0, 1, 0, 0),
                vec4(0, 0, 1, 0),
                vec4(a, 1));

  // calculate new origin
  //_origin = t * vec4(_origin, 1);
  transform(t);
  _mat_translation = _mat_translation * t;
  calculate_inverse_mat();
}

void Object::transform_point(mat4 transformation, vec3 *point) {
  *point =  transformation * vec4(*point, 1);
}

void Object::transform_matrices(mat4 transformation) {
  _mat_translation = transformation * _mat_translation;
  _mat_rotation = transformation * _mat_rotation;

  calculate_inverse_mat();
}

void Object::transform(mat4 transformation) {
  transform_point(transformation, &_origin);
}

// rotate x,y,z degree around the respective axes.
void Object::rotate(vec3 axis, float degree) {
  axis = glm::normalize(axis); // recommended for glm library
  mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(degree), axis);

  // move to origin -> rotate -> move back
  transform(_mat_translation * rot * _mat_inv_translation);
  // transform direction vector
  transform_point(rot, &_direction);

  // update rotation matrices
  _mat_rotation = rot * _mat_rotation;
  calculate_inverse_mat();
}

vec3 Object::origin_to_virtual(vec3 point) {
  mat4 transform = _mat_translation * _mat_rotation;
  vec3 res = point;
  transform_point(transform, &res);
  return res;
}

vec3 Object::virtual_to_origin(vec3 point) {
  mat4 transform = _mat_inv_rotation * _mat_inv_translation;
  vec3 res = point;
  transform_point(transform, &res);
  return res;
}

// updates rotation matrix and sets new direction
void Object::calculate_direction(vec3 new_dir) {
  // TODO implement
  vec3 axis = glm::cross(_direction, new_dir);
  
  float degree = glm::acos(glm::dot(new_dir, _direction) / glm::length(new_dir) * glm::length(_direction));

  // if new_dir points to exactly th oposite direction axis = 0, 0, 0
  // TODO check if this is right or nonsense
  if (axis.x == 0 && axis.y == 0 && axis.z == 0) {
    axis = vec3(0, 1, 0);
  }

  _mat_rotation = glm::rotate(_mat_rotation, degree, axis);
  calculate_inverse_mat();
  transform_point(_mat_rotation, &_direction);
}