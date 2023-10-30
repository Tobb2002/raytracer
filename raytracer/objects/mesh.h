/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#ifndef MESH_H_
#define MESH_H_

#include <iostream>
#include <vector>
#include "triangle.h"

struct intersection {
  vec3 point;
  Triangle triangle;
};

class Mesh {
 public:
  Mesh(std::string input_file, vec3 origin);
  ~Mesh();

  void print_triangles(void);
  int get_size(void);

  Triangle get_triangle(int i);
  intersection get_closest_intersection(Ray ray);

 private:
  bool _triangle_exists;
  std::vector<Triangle> _triangles;
  int _size;
  vec3 _origin;

  void read_from_obj(std::string input_file);
};


#endif  // MESH_H_
