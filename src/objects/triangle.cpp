/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "triangle.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

/**
 * @brief Construct a new Triangle:: Triangle object
 *
 */
Triangle::Triangle() {
  // initialize trianlge with all points zero.
  for (int i = 0; i < 3; i++) {
    _p[i] = vec3(0, 0, 0);
    _p_uv[i] = vec2(-1, -1);
  }
  _normal = vec3(0, 0, 0);
  _middle_point = calculate_middle();
}

Triangle::Triangle(vec3 points[3], uint material_id) {
  for (int i = 0; i < 3; i++) {
    _p[i] = points[i];
    _p_uv[i] = vec2(-1, -1);
  }
  _normal = calculate_normal();
  _material_id = material_id;
}

Triangle::Triangle(vec3 points[3], uint material_id, vec2 uv_coordinates[3]) {
  for (int i = 0; i < 3; i++) {
    _p[i] = points[i];
    _p_uv[i] = uv_coordinates[i];
  }
  _normal = calculate_normal();
  _material_id = material_id;
}

vec3 Triangle::calculate_normal(void) {
  // take the cross product of p1,p2 and p2,p3
  vec3 v1 = _p[0] - _p[1];
  vec3 v2 = _p[1] - _p[2];

  return glm::normalize(glm::cross(v1, v2));
}

vec3 Triangle::calculate_normal_interpolated(vec3 uvw) {
  return glm::normalize((1 - uvw.y - uvw.z) * _p_normal[0] +
                        uvw.y * _p_normal[1] + uvw.z * _p_normal[2]);
}
vec2 Triangle::calculate_texture_interpolated(vec3 uvw) {
  return (1 - uvw.y - uvw.z) * _p_uv[0] + uvw.y * _p_uv[1] + uvw.z * _p_uv[2];
}

vec3 Triangle::calculate_middle(void) {
  vec3 res;
  res.x = (_p[0].x + _p[1].x + _p[2].x) / 3;
  res.y = (_p[0].y + _p[1].y + _p[2].y) / 3;
  res.z = (_p[0].z + _p[1].z + _p[2].z) / 3;
  return res;
}

// calculate intersection and give back t.
// if no intersection return -1.
TriangleIntersection Triangle::intersect_triangle(const Ray& ray) {
  /*
   * returns intersection Point beetween ray and trianlge.
   * if intersection is outside of triangle return vec(0,0,0) since that's the
   * camera origin.
   */

  vec3 e[2];

  e[0] = _p[1] - _p[0];
  e[1] = _p[2] - _p[0];

  vec3 s = ray.get_origin() - _p[0];
  vec3 d = ray.get_direction();

  float p1 = (1 / (glm::dot(glm::cross(d, e[1]), e[0])));

  vec3 p2 = vec3((glm::dot(glm::cross(s, e[0]), e[1])),
                 (glm::dot(glm::cross(d, e[1]), s)),
                 (glm::dot(glm::cross(s, e[0]), d)));

  // res: (t, u, v) barycentric coordinates of hit point
  vec3 res = p1 * p2;

  bool found = true;

  // check if intersection is inside triangle and in front of Camera(t >0)
  if (!(1 - res[1] - res[2] >= 0 && res[1] >= 0 && res[2] >= 0 &&
        res[0] >= 0)) {
    // return -1 if not
    found = false;
  }

  vec3 normal = _normal;
  if (_enable_smooth_normals) {
    normal = calculate_normal_interpolated(res);
  }
  // textures enabled
  vec2 texture = vec2(-1);
  if (_p_uv[0].x != -1) {
    texture = calculate_texture_interpolated(res);
  }

  TriangleIntersection i = {found,  res[0],       ray.get_point(res[0]),
                            normal, _material_id, texture};

  return i;
}

// ----- setters ------
void Triangle::set_vertex_normals(vec3 normals[3]) {
  _enable_smooth_normals = true;
  for (int i = 0; i < 3; i++) {
    _p_normal[i] = normals[i];
  }
}
void Triangle::set_vertex_texture(vec2 texture_uv[3]) {
  // _enable_texture = true;
  _p_uv[0] = texture_uv[0];
  _p_uv[1] = texture_uv[1];
  _p_uv[2] = texture_uv[2];
  // for (int i = 0; i < 3; i++) {
  //   _p_uv[i] = texture_uv[i];
  // }
}

vec3 Triangle::get_normal() { return _normal; }
vec3 Triangle::get_pos() { return calculate_middle(); }
uint Triangle::get_material(void) { return _material_id; }

void Triangle::set_material(uint material_id) { _material_id = material_id; }

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
  std::cout << "----Triangle---- " << std::endl;
  for (int i = 0; i < 3; i++) {
    std::cout << "p" << i << ": " << glm::to_string(_p[i]) << std::endl;
  }
  std::cout << "material id: " << _material_id << std::endl;
  std::cout << "normal: " << glm::to_string(_normal) << std::endl;
  std::cout << "---------------- " << std::endl;
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
