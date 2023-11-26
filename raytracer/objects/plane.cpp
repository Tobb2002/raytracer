/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "plane.hpp"

#include <glm/gtx/string_cast.hpp>

Plane::Plane() {
  _normal = _direction;
  _two_colored = false;
}

Plane::Plane(vec3 position, vec3 normal, Material material) {
  calculate_direction(normal);

  _origin = position;
  _mat_translation = glm::translate(_mat_translation, position);
  _normal = glm::normalize(normal);
  _material = material;
  _material2 = material;
  _two_colored = true;

  calculate_inverse_mat();
  print_matrices();
}
Plane::Plane(vec3 position, vec3 normal, Material material1, Material material2) {
  calculate_direction(normal);

  _origin = position;
  _mat_translation = glm::translate(_mat_translation, position);
  _normal = glm::normalize(normal);
  _material = material1;
  _material2 = material2;
  _two_colored = true;

  calculate_inverse_mat();
}

// calculate The intersection Point with a Ray.
Intersection Plane::intersect(Ray ray) {
  float t = -(glm::dot(ray.get_origin() - _origin, _normal)
            / glm::dot(ray.get_direction(), _normal));

  bool found = false;
  if (t >= 0) {
    found = true;
  }
  Intersection i = {found, t, ray.get_point(t), _normal, get_material(ray.get_point(t))};

  return i;
}

Material Plane::get_material(vec3 point) {
  // calculate the point relative to not transformed matrix
  //vec3 point_origin_mod = virtual_to_origin(point);
  vec3 point_origin = virtual_to_origin(point);
  vec3 point_origin_mod = glm::mod(point_origin, 4.f);

  //std::cout << "point" << glm::to_string(point) << "\n";
  //std::cout << "point_or_mod" << glm::to_string(point_origin_mod) << "\n";



  if (!_two_colored) {
    return _material;
  }
  // return diffrent colors for x,y specific values
  if ((point_origin.x < 0.1 && point_origin.x > -0.1) ||
      (point_origin.y < 0.1 && point_origin.y > -0.1)) {
    return {.color= vec3(0, 1, 0)};
  }
  if ((point_origin_mod.x < 2 &&
      point_origin_mod.y < 2)
      ||
      (point_origin_mod.x > 2 &&
      point_origin_mod.y > 2)) {
    return _material;
  }
  else {
    return _material2;
  }
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

  _normal = _direction;
  //transform_point(transformation, &_normal);

}