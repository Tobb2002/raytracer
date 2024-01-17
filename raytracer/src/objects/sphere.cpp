/*
 * Copyright (c) 2024 Tobias Vonier. All rights reserved.
 */

#include "sphere.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

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

void Sphere::set_start_position(vec3 position, float radius) {
  _origin = position;
  _radius = radius;
}

Intersection Sphere::intersect(Ray ray) {
  // sphere equation from raytracing in one weekend
  vec3 oc = ray.get_origin() - _origin;
  //vec3 oc = vec3(0, 0, 0) - _origin;
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

Material Sphere::get_material(vec3 point) { return _material; }

vec3 Sphere::calculate_normal(vec3 surface_point) {
  return glm::normalize(surface_point - _origin);
}
