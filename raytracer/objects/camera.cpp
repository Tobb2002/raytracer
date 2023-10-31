/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "camera.h"
#include <stdexcept>

#include <iostream>
#include <glm/gtx/string_cast.hpp>

Camera::Camera(float resolution_x, float resolution_y) {
  _resolution = vec2(resolution_x, resolution_y);
  _aspect_ratio = resolution_x / resolution_y;


  // set standart zoom (sensor_size)
  set_sensor_size(3, 3);

}

// geters
vec2 Camera::get_resolution() {
  return _resolution;
}
// setters
void Camera::set_sensor_size(int x, int y) {
  _sensor_size = vec2(x, y);

  _pixel_size = vec2(
    _sensor_size.x / _resolution.x,
    _sensor_size.y / _resolution.y);
}

// transfomation beetween diffrent spaces
vec2 Camera::pixel_to_image_pos(vec2 pixel) {
  /*
   * returns pixel position in world units relative to image plane.
   */
  // check if pixel is valid
  if (pixel.x < 0 ||
      pixel.y < 0 ||
      pixel.x >= _resolution.x ||
      pixel.y >= _resolution.y) {
    throw std::invalid_argument("pixel not in given resolution");
  }

  vec2 res = vec2(
    0.5 * _pixel_size.x + _pixel_size.x * pixel.x,
    0.5 * _pixel_size.y + _pixel_size.y * pixel.y);

  return res;
}

vec3 Camera::image_to_world(vec2 pos_image) {
  vec3 start = vec3(
    -0.5 * _sensor_size.x,
    -0.5 * _sensor_size.y,
    -1.f);

  vec3 p = vec3(pos_image.x, pos_image.y, 0.f);

  return start + p;
}

Ray Camera::get_ray(vec2 pixel) {
  vec2 pos_image = pixel_to_image_pos(pixel);
  vec3 direction = image_to_world(pos_image);
  Ray res = Ray(_origin, direction);

  return res;
}
