/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "pointlight.h"

Pointlight::Pointlight(vec3 position, vec3 color) {
  _position = position;
  _color = color;
}

vec3 Pointlight::get_light_direction(vec3 point) {
  return glm::normalize(point - _position);
}

vec3 Pointlight::get_color() { return _color; }