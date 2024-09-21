/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <CImg.h>

#include <string>

#define FLIP_HORIZONTAL
#include "glm/glm.hpp"

using glm::vec2, glm::vec3;

using cimg_library::CImg, cimg_library::CImgDisplay;

class Texture {
 public:
  Texture();
  explicit Texture(std::string path_to_image);

  void show_image(void);

  void load_image(std::string path_to_image);

  vec3 get_color_uv(vec2 position_uv);
  vec3 get_color_absolute(vec2 position);

  vec3 get_normal_uv(vec2 position_uv);

  vec2 get_dimensions();

 private:
  CImg<unsigned char> _texture;
};
