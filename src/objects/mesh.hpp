/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <iostream>
#include <vector>

#include "box.hpp"
#include "bvh.hpp"
#include "object.hpp"
#include "triangle.hpp"

class Mesh : public Object {
 public:
  Mesh(std::string input_file, vec3 origin);
  Mesh(std::string input_file, vec3 origin, Material material);

  Mesh(const Mesh& old_mesh);
  ~Mesh();

  /***** Print Debug information *****/
  void print_triangles(void);
  void print_bounding_box(void);
  void print(void) override;

  /***** Transformations *****/

  void apply_transform(mat4 transformation) override;

  /***** getters *****/
  int get_size(void);
  Triangle get_triangle(int i);

  /***** Functions *****/
  Intersection intersect(const Ray& ray) override;

 private:
  std::vector<Triangle> _triangles;
  bool _triangle_exists;
  int _size;
  vec3 _origin;
  Box _bounding_box;
  BVH _bvh;

  bool _enable_smooth_shading = true;

  Material _material;

  void update_bounding_box(Triangle* t);
  void read_from_obj(std::string input_file);
};
