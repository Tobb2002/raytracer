/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "plane.hpp"

#include <glm/gtx/string_cast.hpp>

Plane::Plane() {
  _normal = _direction;
  _color = vec3(100, 100, 100);
}

Plane::Plane(vec3 position, vec3 normal, vec3 color) {
  _origin = position;
  _normal = glm::normalize(normal);
  _color = color;
}

// calculate The intersection Point with a Ray.
Intersection Plane::intersect(Ray ray) {
  float t = -(glm::dot(ray.get_origin() - _origin, _normal)
            / glm::dot(ray.get_direction(), _normal));

  bool found = false;
  if (t >= 0) {
    found = true;
  }
  Intersection i = {found, t, ray.get_point(t), _normal, _color};

  return i;
}

void Plane::print(void) {
  Object::print();
  std::cout << "-----Plane------\n";
  std::cout << "Position: " << glm::to_string(_origin) << "\n";
  std::cout << "Normal: " << glm::to_string(_normal) << "\n";
  std::cout << "----------------\n";
}

/*
 * Transformation
 */

void Plane::transform(mat4 transformation) {
  Object::transform(transformation);

  transform_point(transformation, &_normal);

}