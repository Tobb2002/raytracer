/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include <string>

using glm::vec2;
using glm::vec3;

struct point {
  int x;
  int y;
};

/**
 * @brief Datatype for handeling image data.
 *
 * This Class handles images of different resolutions
 * and can write these to the file system as ppm files.
 */
class Image {
 public:
  Image(int resolution_x, int resolution_y);
  ~Image();

  void write_to_file(std::string filename);

  // --- configure scene ---
  void set_pixel(point pixel, vec3 color);
  void apply_tonemapping(float middle_gray);

 private:
  float get_luminance(vec3 color);
  float get_average_luminance();
  void apply_luminance(vec3* color, float luminance);
  void initialize_matrix(vec3 standart_color);
  void delete_matrix();

  // --- data ---
  int _resolution[2];
  vec3** _matrix;
};
