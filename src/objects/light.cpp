/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "light.hpp"

Light::Light(vec3 position, vec3 color) {
  _origin = position;
  _color = color;
}

Light::Light(vec3 position, float intensity) {
  _origin = position;
  _color = vec3(intensity);
}

vec3 Light::get_light_direction(vec3 point) const {
  return glm::normalize(_origin - point);
}

float Light::get_distance(vec3 point) const {
  return glm::distance(_origin, point);
}

vec3 Light::get_color() const { return _color; }

