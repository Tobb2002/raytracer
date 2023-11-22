/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "triangle.hpp"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

Triangle::Triangle() {
  // initialize trianlge with all points zero.
  for (int i = 0; i < 3; i++) {
    _p[i] = vec3(0, 0, 0);
  }
  _normal = vec3(0, 0, 0);
  _color = vec3(0, 0, 0);
}

Triangle::Triangle(vec3 points[3], vec3 color) {
  for (int i = 0; i < 3; i++) {
    _p[i] = points[i];
  }
  _normal = calculate_normal();
  _color = color;
}

vec3 Triangle::calculate_normal(void) {
  // take the cross product of p1,p2 and p2,p3
  vec3 v1 = _p[0] - _p[1];
  vec3 v2 = _p[1] - _p[2];

  return glm::normalize(glm::cross(v1, v2));
}

// calculate intersection and give back t.
// if no intersection return -1.
Intersection Triangle::intersect(Ray ray) {
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
    1 / (glm::dot(glm::cross(d, e[1]), e[0])));

  vec3 p2 = vec3(
    (glm::dot(glm::cross(s, e[0]), e[1])),
    (glm::dot(glm::cross(d, e[1]), s)),
    (glm::dot(glm::cross(s, e[0]), d)));

  // res = vec(t, b1, b2);
  vec3 res = p1 * p2;

  bool found = true;
  // check if intersection is inside triangle and in front of Camera(t >0)
  if (!(1 - res[1] - res[2] >= 0 &&
      res[1] >= 0 &&
      res[2] >= 0 &&
      res[0] >= 0)) {
    // return -1 if not
    found = false;
  }

  Intersection i = {found, res[0], ray.get_point(res[0]), _normal, _color};

  return i;
}

void Triangle::move(vec3 vec) {
  for (int i = 0; i < 3; i++) {
    _p[i] +=vec;
  }
}

vec3 Triangle::get_color() {
  return _color;
}
vec3 Triangle::get_normal() { return _normal; }

void Triangle::set_color(vec3 color) {
  _color = color;
}

void Triangle::print() {
  std::cout << "----Triangle---- " <<  std::endl;
  for (int i = 0; i < 3; i++) {
    std::cout << "p" << i << ": " << glm::to_string(_p[i]) << std::endl;
  }
  std::cout << "color: " << glm::to_string(_color) << std::endl;
  std::cout << "normal: " << glm::to_string(_normal) << std::endl;
  std::cout << "---------------- " <<  std::endl;
}
