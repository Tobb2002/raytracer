/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "plane.hpp"

#include <glm/gtx/string_cast.hpp>

/// @brief Construct a new Plane:: Plane object
Plane::Plane() {
  _point = vec3(0, 0, 1);
  calculate_normal();
  _two_colored = false;
}

void Plane::set_start_position(vec3 position, vec3 normal) {
  _point = vec3(0, 0, 1);
  _origin = vec3(0, 0, 0);
  std::cout << "norm: " << glm::to_string(normal) << "\n";
  calculate_direction(normal, vec3(0, 0, 1));

  // move to position
  mat4 t = _transform.add_translation(position);
  apply_transform(t);
}

/**
 * @brief Construct a new Plane:: Plane object
 * 
 * @param position One point on the plane (origin of plane).
 * @param normal normal of the plane.
 * @param material material of the plane.
 */
Plane::Plane(vec3 position, vec3 normal, Material material) {

  set_start_position(position, normal);

  _material = material;
  _material2 = material;
  _two_colored = true;
}

/**
 * @brief Construct a new Plane:: Plane object with two materials
 * 
 * @param position One point on the plane (origin of plane).
 * @param normal normal of the plane.
 * @param material1
 * @param material2
 * 
 * The plane consits of two materials ordere like a chess board.
 */
Plane::Plane(vec3 position,
    vec3 normal,
    Material material1,
    Material material2) {
  set_start_position(position, normal);

  _material = material1;
  _material2 = material2;
  _two_colored = true;
}

/***** Intersection *****/

Intersection Plane::intersect(Ray ray) {
  float t = -(glm::dot(ray.get_origin() - _origin, _normal)
            / glm::dot(ray.get_direction(), _normal));

  bool found = false;
  if (t >= 0) {
    found = true;
  }
  Intersection i = {found, t, ray.get_point(t),
      _normal, get_material(ray.get_point(t))};

  return i;
}

/**
 * @brief 
 * 
 * @param point 
 * @return Material 
 */
Material Plane::get_material(vec3 point) {
  if (!_two_colored) {
    return _material;
  }

  // calculate point relative to standart plane (at origin)
  vec3 point_origin = _transform.virtual_to_origin(_transform.transform_point(_view_transform.inv, point));
  vec3 point_origin_mod = glm::mod(point_origin, 4.f);

  // return diffrent colors for x,y specific values
  if ((point_origin.x < 0.1 && point_origin.x > -0.1) ||
      (point_origin.y < 0.1 && point_origin.y > -0.1)) {
    return {.color = vec3(0, 1, 0)};
  }
  if ((point_origin_mod.x < 2 &&
      point_origin_mod.y < 2)
      ||
      (point_origin_mod.x > 2 &&
      point_origin_mod.y > 2)) {
    return _material;
  } else {
    return _material2;
  }
}

/***** Print DEBUG information *****/

void Plane::print(void) {
  Object::print();
  std::cout << "-----Plane------\n";
  std::cout << "Position: " << glm::to_string(_origin) << "\n";
  std::cout << "Point: " << glm::to_string(_point) << "\n";
  std::cout << "Normal: " << glm::to_string(_normal) << "\n";
  std::cout << "----------------\n";
}

/***** Transformation *****/

void Plane::apply_transform(mat4 transformation) {
  Object::apply_transform(transformation);

  _transform.transform_point(transformation, &_point);
  calculate_normal();
}

/**
 * @brief Set new direction and update rotation matrix.
 * 
 * @param new_dir
 */
void Plane::calculate_direction(vec3 new_dir, vec3 old_direction) {
  Rotation rot = _transform.calculate_rotation(new_dir, old_direction);


  mat4 t = _transform.add_rotation(rot.axis, rot.degree);
  std::cout << "t: " << glm::to_string(rot.axis) << "\n";
  std::cout << "deg: " << rot.degree << "\n";
  apply_transform(t);
}

void Plane::calculate_normal() {
  _normal = glm::normalize(_point - _origin);
}
