/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <glm/glm.hpp>
#include <string>

using glm::vec3;
using glm::vec2;

class Image {
 public:
  Image(int resolution_x, int resolution_y);
  ~Image();

  void write_to_file(std::string filename);
 private:
  void initialize_matrix(vec3 standart_color);
  void delete_matrix();
  int _resolution[2];
  vec3** _matrix;
};

#endif  // IMAGE_H_
