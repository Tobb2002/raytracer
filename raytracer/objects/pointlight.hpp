/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <glm/glm.hpp>

using glm::vec3;

class Pointlight {
 public:
  Pointlight(vec3 position, vec3 color);

  vec3 get_light_direction(vec3 point);
  vec3 get_color();

 private:
  vec3 _position;
  vec3 _color;
};
