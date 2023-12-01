/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "object.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/component_wise.hpp>

using glm::vec4;

Object::Object() {
  _origin = vec3(0, 0, 0);
  _direction = vec3(0, 0, 1);

  initialize_matrices();
  calculate_inverse_mat();
}

/**
 * @brief Calculate intersection with a Ray.
 * 
 * @param ray 
 * @return Intersection 
 * 
 * This function should be overriden by inheriting Objects.
 */
Intersection Object::intersect(Ray ray) {
  Material material;
  return {false, 0, vec3(0, 0, 0), vec3(0, 0, 0), material};
}

/**
 * @brief Check if Ray intersects.
 * 
 * @param ray 
 * @param t_max maximum length of Ray to check.
 * @return true if Ray intersects.
 * @return false else
 */
bool Object::intersect_bool(Ray ray, float t_max) {
  Intersection i = intersect(ray);

  if (i.found && i.t < t_max) {
    return true;
  }

  return false;
}

/***** Print DEBUG information *****/

/**
 * @brief Print all important information of the Object.
 * 
 * This function should be ovveridden by inheriting objects.
 */
void Object::print(void) {
  std::cout << "-----Object-----\n";
}

/**
 * @brief Print all the transformation matrices.
 */
void Object::print_matrices(void) {
  std::cout << "-----Object-----\n";
  std::cout << "-----Matrix-----\n";
  std::cout << "translation:\n" << glm::to_string(_mat_translation) << "\n";
  std::cout << "rotation:\n" << glm::to_string(_mat_rotation) << "\n";
  std::cout << "inv translation:\n" << glm::to_string(_mat_inv_translation) <<
      "\n";
  std::cout << "inv rotation:\n" << glm::to_string(_mat_inv_rotation) << "\n";
  std::cout << "----------------\n";
}

/*
 * Transformation
 */

/**
 * @brief Initialize transformation matrices to identity matrices.
 */
void Object::initialize_matrices(void) {
  // matrices should be identity matrix at beginning.
  _mat_translation = glm::mat4(1.0);
  _mat_rotation = glm::mat4(1.0);
  _mat_scale = glm::mat4(1.0);

  calculate_inverse_mat();
}


/**
 * @brief calculate the inverse for all transformation matrices.
 */
void Object::calculate_inverse_mat(void) {
  // translation T^-1(t) = T(-t)
  _mat_inv_translation =  mat4(vec4(1, 0, 0, 0),
                               vec4(0, 1, 0, 0),
                               vec4(0, 0, 1, 0),
                               vec4(_mat_translation[3][0] * -1,
                                    _mat_translation[3][1] * -1,
                                    _mat_translation[3][2] * -1,
                                    1));
  // rotation R^-1 = transpose(Rx)
  _mat_inv_rotation = glm::transpose(_mat_rotation);
  // scale S^-1 = S(1/s)
  // _mat_inv_scale = glm::compScale
}

/**
 * @brief Move Object.
 * 
 * @param a vector to move the object.
 */
void Object::move(vec3 a) {
  std::cout << "moving object\n";

  mat4 t = _transform.add_translation(a);
  apply_transform(t);
}


/**
 * @brief Apply transformation to Object.
 * 
 * @param transformation 
 */
void Object::apply_transform(mat4 transformation) {
  _transform.transform_point(transformation, &_origin);
}

/**
 * @brief Rotate Object around it's own origin.
 * 
 * @param axis Axis for the rotation.
 * @param degree Angle to rotate.
 */
void Object::rotate(vec3 axis, float degree) {
  apply_transform(_transform.add_rotation(axis, degree));
  
  // TODO remove when plane updatet
  mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(degree), axis);
  _transform.transform_point(rot, &_direction);
}
