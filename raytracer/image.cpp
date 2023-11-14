/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "image.hpp"
#include <fstream>

Image::Image(int resolution_x, int resolution_y) {
  _resolution[0] = resolution_x;
  _resolution[1] = resolution_y;

  // initialize all pixels to light-blue
  initialize_matrix(vec3(0, 0, 0));
}

Image::~Image() {
  delete_matrix();
}

// matrix operations
void Image::initialize_matrix(vec3 standart_color) {
  _matrix = new vec3* [_resolution[0]];
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
  for (int i = 0; i< _resolution[0]; i++) {
    delete[] _matrix[i];
  }
  delete[] _matrix;
}

void Image::set_pixel(point pixel, vec3 color) {
  _matrix[pixel.x][pixel.y] = color;
}

void Image::write_to_file(std::string filename) {
  /**
   * Function that outputs given Picture in ppm format.
   * Parameters:
   *  picture: <type>
   *  location: <string>
   */

  // initialise File
  std::ofstream file(filename);

  // File header
  file << "P3\n" << _resolution[0] << " " << _resolution[1] << "\n255\n";

  // Picture Content
  for (int y = _resolution[1]-1; y >= 0; y--) {
    for (int x = 0; x < _resolution[0]; x++) {
      vec3 color = _matrix[x][y];
      file << static_cast<int>(color[0]) << " "
           << static_cast<int>(color[1]) << " "
           << static_cast<int>(color[2]) << "\n";
    }
  }
}
