/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "texture.hpp"

#include <iostream>

Texture::Texture() {}

Texture::Texture(std::string path_to_image) {
  _texture = CImg<unsigned char>(path_to_image.c_str());
}

void Texture::show_image(void) {
  CImgDisplay main_display(_texture, "Ich bin ein Bild");
  while (!main_display.is_closed()) {
    main_display.wait();
  }
}

void Texture::load_image(std::string path_to_image) {
  _texture = CImg<unsigned char>(path_to_image.c_str());
}

vec3 Texture::get_color_absolute(vec2 position_xy) {
  vec3 colors =
      vec3(static_cast<int>(_texture(position_xy.x, position_xy.y, 0, 0)),
           static_cast<int>(_texture(position_xy.x, position_xy.y, 0, 1)),
           static_cast<int>(_texture(position_xy.x, position_xy.y, 0, 2)));
  return colors;
}

// only acepts values between 0-1
vec3 Texture::get_color_uv(vec2 position_uv) {
  position_uv = glm::abs(position_uv - vec2(static_cast<int>(position_uv.x),
                                            static_cast<int>(position_uv.y)));

// flip y direction
#ifdef FLIP_HORIZONTAL
  position_uv.y = 1 - position_uv.y;
#endif
  position_uv.x = static_cast<int>(position_uv.x * _texture.width());
  position_uv.y = static_cast<int>(position_uv.y * _texture.height());
  vec3 colors = get_color_absolute(position_uv);
  colors *= 1.f / 255;
  return colors;
}
vec3 Texture::get_normal_uv(vec2 position_uv) {
  if (position_uv.x > 1) {
    position_uv.x = 0.99;
  }
  if (position_uv.y > 1) {
    position_uv.y = 0.99;
  }
  if (position_uv.x < 0) {
    position_uv.x = 0;
  }
  if (position_uv.y < 0) {
    position_uv.y = 0;
  }
// flip y direction
#ifdef FLIP_HORIZONTAL
  position_uv.y = 1 - position_uv.y;
#endif
  position_uv.x = static_cast<int>(position_uv.x * _texture.width());
  position_uv.y = static_cast<int>(position_uv.y * _texture.height());
  vec3 colors =
      vec3(static_cast<int>(_texture(position_uv.x, position_uv.y, 0, 0)),
           static_cast<int>(_texture(position_uv.x, position_uv.y, 0, 1)),
           static_cast<int>(_texture(position_uv.x, position_uv.y, 0, 2)));
  colors *= 1.f / 255;
  vec3 normal = colors * vec3(2) - vec3(1);
  return normal;
}

vec2 Texture::get_dimensions() {
  return vec2(_texture.width(), _texture.height());
}
