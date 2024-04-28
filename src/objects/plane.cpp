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
Plane::Plane(vec3 position,
             vec3 normal,
             Material material1,
             Material material2,
             vec2 size) {
  set_start_position(position, normal);

  _material = material1;
  _material2 = material2;
  _two_colored = true;
  _enable_size = true;
  _size = size;
}

Plane::Plane(vec3 pos, vec3 normal, Material material,
             vec2 size, std::string path_to_file) {
  set_start_position(pos, normal);

  _material = material;
  _size = size;
  _enable_size = true;
  _texture.load_image(path_to_file);
  _textured = true;
}
/***** settings *****/
void Plane::set_axis(bool enable) {
  _axis_enable = enable;
}

/***** Intersection *****/

Intersection Plane::intersect(const Ray& ray) {
  float t = -(glm::dot(ray.get_origin() - _origin, _normal)
            / glm::dot(ray.get_direction(), _normal));

  bool found = false;
  if (t >= 0) {
    found = true;
  }
  vec3 point = ray.get_point(t);
  vec3 point_origin = _transform.virtual_to_origin(
    _transform.transform_point(_view_transform.inv, point));
  if (_enable_size && (point_origin.x > _size.x ||
      point_origin.x < -_size.x ||
      point_origin.y > _size.y ||
      point_origin.y < -_size.y)) {
    found = false;
  }
  Intersection i = {found, t, point,
      _normal, get_material(point)};

  return i;
}

/**
 * @brief 
 * 
 * @param point 
 * @return Material 
 */
Material Plane::get_material(vec3 point) {
  // calculate point relative to standart plane (at origin)
  vec3 point_origin =
    _transform.virtual_to_origin(
      _transform.transform_point(_view_transform.inv, point));

  if (_textured) {
    // calculate pointuv
    vec2 point_uv = vec2((point_origin.x + _size.x) / _size.x / 2,
                         (point_origin.y + _size.y) / _size.y / 2);
    _material.color = _texture.get_color_uv(point_uv);
    return _material;
  }

  if (!_two_colored) {
    return _material;
  }

  // squares of two materials
  vec3 point_origin_mod = glm::mod(point_origin, 4.f);

  // draw axis of worldspace
  if (_axis_enable) {
    if ((point_origin.x < 0.1 && point_origin.x > -0.1) ||
        (point_origin.y < 0.1 && point_origin.y > -0.1)) {
      return {.color = vec3(0, 1, 0)};
    }
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
  apply_transform(t);
}

void Plane::calculate_normal() {
  _normal = glm::normalize(_point - _origin);
}
