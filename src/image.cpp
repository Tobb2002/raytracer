/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "image.hpp"

#include <fstream>
#include <iostream>

/**
 * @brief Construct a new Image:: Image object
 *
 * @param resolution_x set's the image width.
 * @param resolution_y set's the image height.
 */
Image::Image(int resolution_x, int resolution_y) {
  _resolution[0] = resolution_x;
  _resolution[1] = resolution_y;

  // initialize all pixels to light-blue
  initialize_matrix(vec3(0, 0, 0));
}

Image::~Image() { delete_matrix(); }

/**
 * @brief Set color value for a specific pixel.
 *
 * @param pixel pixel cordinate as {x, y}.
 * @param color new color value.
 */
void Image::set_pixel(point pixel, vec3 color) {
  // check if color is valid
  for (int i = 0; i < 3; i++) {
    if (color[i] < 0) {
      color[i] = 0;
    }
  }
  _matrix[pixel.x][pixel.y] = color;
}

void Image::apply_tonemapping(float middle_gray) {
  float avg_luminance = get_average_luminance();
  for (int y = _resolution[1] - 1; y >= 0; y--) {
    for (int x = 0; x < _resolution[0]; x++) {
      float l_p = middle_gray / avg_luminance * get_luminance(_matrix[x][y]);

      float new_luminance = l_p / (1 + l_p);

      apply_luminance(&_matrix[x][y], new_luminance);
    }
  }
}

/**
 * @brief Write image to filesystem
 *
 * @param filename Path of outputfile.
 */
void Image::write_to_file(std::string filename) {
  // initialise File
  std::ofstream file(filename);

  // File header
  file << "P3\n" << _resolution[0] << " " << _resolution[1] << "\n255\n";

  // Picture Content
  for (int y = _resolution[1] - 1; y >= 0; y--) {
    for (int x = 0; x < _resolution[0]; x++) {
      vec3 color = _matrix[x][y];
      file << static_cast<int>(color[0]) << " " << static_cast<int>(color[1])
           << " " << static_cast<int>(color[2]) << "\n";
    }
  }
}

/**
 * @brief initialize the image matrix.
 *
 * @param standart_color start pixel value.
 */
float Image::get_luminance(vec3 color) {
  // formula from
  // https://stackoverflow.com/questions/596216/formula-to-determine-perceived-brightness-of-rgb-color
  return 0.2126 * color.x + 0.7152 * color.y + 0.0722 * color.z;
}
float Image::get_average_luminance() {
  float avg_luminance = 0;
  for (int y = _resolution[1] - 1; y >= 0; y--) {
    for (int x = 0; x < _resolution[0]; x++) {
      avg_luminance += glm::log(get_luminance(_matrix[x][y]));
    }
  }
  avg_luminance /= (_resolution[0] * _resolution[1]);
  return glm::exp(avg_luminance);
}
void Image::apply_luminance(vec3* color, float luminance) {
  (*color).x *= luminance;
  (*color).y *= luminance;
  (*color).z *= luminance;
}

void Image::initialize_matrix(vec3 standart_color) {
  _matrix = new vec3*[_resolution[0]];
  for (int i = 0; i < _resolution[0]; i++) {
    _matrix[i] = new vec3[_resolution[1]];
  }

  // set initial values
  for (int x = 0; x < _resolution[0]; x++) {
    for (int y = 0; y < _resolution[1]; y++) {
      _matrix[x][y] = standart_color;
    }
  }
}

void Image::delete_matrix() {
  // clean up the pointer matrix
  for (int i = 0; i < _resolution[0]; i++) {
    delete[] _matrix[i];
  }
  delete[] _matrix;
}
