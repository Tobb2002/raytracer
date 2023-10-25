/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "./triangle.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>


Triangle::Triangle(vec3 points[3], vec3 color) {
  for (int i = 0; i < 3; i++) {
    _p[i] = points[i];
  }
  _color = color;
}

vec3 Triangle::intersect(Ray ray) {
  vec3 e[2];
  
  e[0] = _p[1] - _p[0];
  e[1] = _p[2] - _p[0];

  vec3 s = ray.get_origin() - _p[0];
  vec3 d = ray.get_direction();

  vec3 v = vec3(
    (glm::cross(s, e[1]) * e[2]),
    (glm::cross(d, e[2]) * s),
    (glm::cross(s, e[1]) * d)
  )



}

void Triangle::print(){
  std::cout << "----Triangle---- " <<  std::endl;
  for (int i = 0; i < 3; i++) {
    std::cout << "p" << i << ": " << glm::to_string(_p[i]) << std::endl;
  }
  std::cout << "color: " << glm::to_string(_color) << std::endl;
  std::cout << "---------------- " <<  std::endl;
}