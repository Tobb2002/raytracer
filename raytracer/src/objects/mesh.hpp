/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <iostream>
#include <vector>
#include "triangle.hpp"
#include "box.hpp"
#include "object.hpp"

class Mesh : public Object{
 public:
  Mesh(std::string input_file, vec3 origin);
  Mesh(std::string input_file, vec3 origin, Material material);
  ~Mesh();

  void print_triangles(void);
  void print_bounding_box(void);
  void print(void) override;
  int get_size(void);

  // transformations
  void move(vec3 vec) override;

  void transform(mat4 transformation) override;


  Triangle get_triangle(int i);
  Intersection intersect(Ray ray) override;

 private:
  bool _triangle_exists;
  std::vector<Triangle> _triangles;
  int _size;
  vec3 _origin;
  Box _bounding_box;

  Material _material;

  void update_bounding_box(Triangle * t);
  void read_from_obj(std::string input_file);
};
