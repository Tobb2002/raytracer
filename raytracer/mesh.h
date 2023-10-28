/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#ifndef MESH_H_
#define MESH_H_

#include "triangle.h"
//#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "lib/tiny_obj_loader.h"
#include <iostream>

class Mesh {
 public:
  Mesh(std::string input_file);
  ~Mesh();

  void print_triangles(void);


 private:
  Triangle *_triangles;
  int _size;

  void delete_triangles(void);

  void read_from_obj(std::string input_file);
};


#endif  //MESH_H_
