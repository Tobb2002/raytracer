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
  /* 
   * returns intersection Point beetween ray and trianlge.
   * if intersection is outside of triangle return vec(0,0,0) since that's the camera origin.
   */
  vec3 e[2];
  
  e[0] = _p[1] - _p[0];
  e[1] = _p[2] - _p[0];

  vec3 s = ray.get_origin() - _p[0];
  vec3 d = ray.get_direction();

  float p1 = (
    1 / (glm::dot(glm::cross(d, e[1]), e[0]))
  );
  vec3 p2 = vec3(
    (glm::dot(glm::cross(s, e[0]), e[1])),
    (glm::dot(glm::cross(d, e[1]), s)),
    (glm::dot(glm::cross(s, e[0]), d)));
  
  // res = vec(t, b1, b2);
  vec3 res = p1 * p2; 

  // check if intersection is inside triangle and in front of Camera(t >0)
  
  // intersection outside of triangle
  if (!(1 - res[1] - res[2] >= 0 && res[1] >= 0 && res[2] >= 0 && res[0] >= 0)) {
    res = vec3(0, 0, 0);
  }

  return res;
}

void Triangle::print(){
  std::cout << "----Triangle---- " <<  std::endl;
  for (int i = 0; i < 3; i++) {
    std::cout << "p" << i << ": " << glm::to_string(_p[i]) << std::endl;
  }
  std::cout << "color: " << glm::to_string(_color) << std::endl;
  std::cout << "---------------- " <<  std::endl;
}