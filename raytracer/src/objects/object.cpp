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
 * @brief Print all the transformation infos.
 */
void Object::print_matrices(void) {
  _transform.print();
}

/***** Transformations *****/

/**
 * @brief Move Object.
 * 
 * @param a vector to move the object.
 */
void Object::move(vec3 a) {
  mat4 t = _transform.add_translation(a);
  transform(t);
}


/**
 * @brief Apply transformation to Object.
 * 
 * @param transformation 
 */
void Object::apply_transform(mat4 transformation) {
  _transform.transform_point(transformation, &_origin);
}

void Object::update_view_transform(Transformation view_transform) {
  apply_transform(_view_transform.inv);
  _view_transform = view_transform;
  apply_transform(_view_transform.mat);
}

/**
 * @brief Rotate Object around it's own origin.
 * 
 * @param axis Axis for the rotation.
 * @param degree Angle to rotate.
 */
void Object::rotate(vec3 axis, float degree) {
  transform(_transform.add_rotation(axis, degree));
}

void Object::rotate(vec3 point, vec3 axis, float degree) {
  transform(_transform.add_rotation(point, axis, degree));
}

void Object::transform(mat4 t) {
  apply_transform(_view_transform.inv);
  apply_transform(t);
  apply_transform(_view_transform.mat);
}
