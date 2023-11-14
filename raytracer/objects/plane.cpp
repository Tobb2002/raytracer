/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "plane.hpp"

#include <glm/gtx/string_cast.hpp>

Plane::Plane() {
  _position = vec3(0, 0, 0);
  _normal = vec3(0, 1, 0);
  _color = vec3(100, 100, 100);
}

Plane::Plane(vec3 position, vec3 normal, vec3 color) {
  _position = position;
  _normal = glm::normalize(normal);
  _color = color;
}

// calculate The intersection Point with a Ray.
Intersection Plane::intersect(Ray ray) {
  float t = -(glm::dot(ray.get_origin() - _position, _normal)
            / glm::dot(ray.get_direction(), _normal));

  bool found = false;
  if (t >= 0) {
    found = true;
  }
  Intersection i = {found, t, ray.get_point(t), _normal, _color};

  return i;
}

bool Plane::intersect_bool(Ray ray) {
  // TODO implement
  return false;
}

void Plane::print(void) {
  std::cout << "-----Plane------\n";
  std::cout << "Position: " << glm::to_string(_position) << "\n";
  std::cout << "Normal: " << glm::to_string(_normal) << "\n";
  std::cout << "----------------\n";
}
