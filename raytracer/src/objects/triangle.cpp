/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "triangle.hpp"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * @brief Construct a new Triangle:: Triangle object
 * 
 */
Triangle::Triangle() {
  // initialize trianlge with all points zero.
  for (int i = 0; i < 3; i++) {
    _p[i] = vec3(0, 0, 0);
  }
  _normal = vec3(0, 0, 0);
  _middle_point = calculate_middle();
}

Triangle::Triangle(vec3 points[3], Material material) {
  for (int i = 0; i < 3; i++) {
    _p[i] = points[i];
  }
  _normal = calculate_normal();
  _material = material;
}

vec3 Triangle::calculate_normal(void) {
  // take the cross product of p1,p2 and p2,p3
  vec3 v1 = _p[0] - _p[1];
  vec3 v2 = _p[1] - _p[2];

  return glm::normalize(glm::cross(v1, v2));
}

vec3 Triangle::calculate_middle(void) {
  // TODO(tobi) implement real middle point
  return _p[0];
}

Intersection Triangle::intersect(Ray *ray) {
  /* 
   * returns intersection Point beetween ray and trianlge.
   * if intersection is outside of triangle return vec(0,0,0) since that's the camera origin.
   */

  vec3 e[2];

  e[0] = _p[1] - _p[0];
  e[1] = _p[2] - _p[0];

  vec3 s = ray->get_origin() - _p[0];
  vec3 d = ray->get_direction();

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

  Intersection i = {found, res[0], ray->get_point(res[0]), _normal, _material};

  return i;
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

  Intersection i = {found, res[0], ray.get_point(res[0]), _normal, _material};

  return i;
}

vec3 Triangle::get_normal() { return _normal; }
vec3 Triangle::get_pos() { return _middle_point; }
Material Triangle::get_material(void) { return _material; }

void Triangle::set_material(Material material) { _material = material; }

vec3 Triangle::get_min_bounding(void) {
  vec3 min = vec3(MAXFLOAT);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (_p[i][j] < min[j]) {
        min[j] = _p[i][j];
      }
    }
  }
  return min;
}

vec3 Triangle::get_max_bounding(void) {
  vec3 max = vec3(-MAXFLOAT);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (_p[i][j] > max[j]) {
        max[j] = _p[i][j];
      }
    }
  }
  return max;
}

void Triangle::print() {
  std::cout << "----Triangle---- " <<  std::endl;
  for (int i = 0; i < 3; i++) {
    std::cout << "p" << i << ": " << glm::to_string(_p[i]) << std::endl;
  }
  std::cout << "color: " << glm::to_string(_material.color) << std::endl;
  std::cout << "normal: " << glm::to_string(_normal) << std::endl;
  std::cout << "---------------- " <<  std::endl;
}

/*
 * transformation
 */

void Triangle::apply_transform(mat4 transformation) {
  Object::apply_transform(transformation);

  for (int i = 0; i < 3; i++) {
    _transform.transform_point(transformation, &_p[i]);
  }
  _transform.transform_point(transformation, &_middle_point);
  _normal = calculate_normal();
}
