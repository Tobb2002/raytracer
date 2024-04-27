/*
 * Copyright (c) 2024 Tobias Vonier. All rights reserved.
 */

#include "sphere.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <cmath>
#include <glm/ext/scalar_constants.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <cmath>

Sphere::Sphere() {
  set_start_position(vec3(0, 0, 0), 1.f);
}

Sphere::Sphere(vec3 pos, float radius) {
  set_start_position(pos, radius);
}

Sphere::Sphere(vec3 pos, float radius, Material material) {
  set_start_position(pos, radius);
  _material = material;
}

Sphere::Sphere(vec3 pos,
               float radius,
               Material material,
               std::string path_to_file) {
  set_start_position(pos, radius);
  _material = material;
  _enable_texture = true;
  _texture.load_image(path_to_file);
}

void Sphere::set_start_position(vec3 position, float radius) {
  _origin = vec3(0, 0, 0);
  move(position);
  _radius = radius;
  _direction_point = position + _radius * vec3(0, 0, 1);
}

void Sphere::enable_texture(bool enable) {
  _enable_texture = enable;
}

Intersection Sphere::intersect(const Ray& ray) {
  // sphere equation from raytracing in one weekend
  vec3 oc = ray.get_origin() - _origin;

  float a = glm::dot(ray.get_direction(), ray.get_direction());
  float b = 2.f * glm::dot(oc, ray.get_direction());
  float c = glm::dot(oc, oc) - (_radius * _radius);

  float discriminant = (b * b) - (4.f * a * c);

  bool found = false;
  float t = -1;

  if (discriminant > 0) {
    t = (-b - glm::sqrt(discriminant)) / (2.f * a);
    if (t > 0) {
      found = true;
    }
  }
  vec3 surface_point = ray.get_point(t);

  Intersection i = {found, t, surface_point,
     calculate_normal(surface_point) , get_material(surface_point)};

  return i;
}

void Sphere::print(void) {
  std::cout << "----Sphere------ " <<  std::endl;
  std::cout << "color: " << glm::to_string(_material.color) << std::endl;
  std::cout << "center: " << glm::to_string(_origin) << std::endl;
  std::cout << "radius: " << _radius << std::endl;
  std::cout << "---------------- " <<  std::endl;
}

Material Sphere::get_material(vec3 point) {
  if (_enable_texture) {
    vec3 n_point = point;
    // reverse view transform
    _transform.transform_point(_view_transform.inv, &n_point);
    // transform sphere back to origin
    n_point = _transform.virtual_to_origin(n_point);

    // normalize normal
    vec3 n = glm::normalize(n_point);

    vec2 point_uv = vec2(atan2(n.x, n.z) / (2*3.141f) + 0.5,
                         -n.y * 0.5 + 0.5);
    _material.color = _texture.get_color_uv(point_uv);
  }

  return _material;
}

void Sphere::apply_transform(mat4 transformation) {
  Object::apply_transform(transformation);

  _transform.transform_point(transformation, &_direction_point);
}
vec3 Sphere::get_direction() {
  return glm::normalize(_direction_point - _origin);
}

vec3 Sphere::calculate_normal(vec3 surface_point) {
  return glm::normalize(surface_point - _origin);
}
