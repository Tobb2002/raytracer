/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "pointlight.hpp"

Pointlight::Pointlight(vec3 position, vec3 color) {
  _position = position;
  _color = color;
}

Pointlight::Pointlight(vec3 position, float intensity) {
  _position = position;
  _color = vec3(intensity);
}

vec3 Pointlight::get_light_direction(vec3 point) {
  return glm::normalize(_position - point);
}

float Pointlight::get_distance(vec3 point) {
  return glm::distance(_position, point);
}

vec3 Pointlight::get_color() { return _color; }
