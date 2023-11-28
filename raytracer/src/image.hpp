/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include <string>

using glm::vec3;
using glm::vec2;

struct point {
  int x;
  int y;
};

class Image {
 public:
  Image(int resolution_x, int resolution_y);
  ~Image();

  void write_to_file(std::string filename);
  void set_pixel(point pixel, vec3 color);
 private:
  void initialize_matrix(vec3 standart_color);
  void delete_matrix();
  int _resolution[2];
  vec3** _matrix;
};
