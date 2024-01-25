/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include "glm/glm.hpp"
#include <CImg.h>

using glm::vec2, glm::vec3;

using cimg_library::CImg, cimg_library::CImgDisplay;

class Texture{
public:
  Texture();
  Texture(std::string path_to_image);

  void show_image(void);

  void load_image(std::string path_to_image);

  vec3 get_color_uv(vec2 position_uv);
  vec3 get_color_absolute(vec2 position);

  vec2 get_dimensions();

private:
  CImg<unsigned char> _texture;
};
