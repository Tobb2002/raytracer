/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <glm/glm.hpp>
#include "object.hpp"

using glm::vec3;

class Pointlight : public Object {
 public:
  Pointlight(vec3 position, vec3 color);
  Pointlight(vec3 position, float intensity);

  vec3 get_light_direction(vec3 point);
  float get_distance(vec3 point);
  vec3 get_color();

 private:
  vec3 _color;
};
