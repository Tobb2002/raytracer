/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "camera.hpp"
#include <stdexcept>

#include <iostream>
#include <glm/gtx/string_cast.hpp>

/**
 * @brief Construct a new Camera:: Camera object
 * 
 * Standart resolution is (1000, 1000).
 * standart sensor size is (1, 1).
 */
Camera::Camera() {
  set_resolution(1000, 1000);
  set_sensor_size(1, 1);
}

/**
 * @brief Construct a new Camera:: Camera object with specific resolution.
 * 
 * @param resolution_x amount of pixels in x direction.
 * @param resolution_y amount of pixels in y direction.
 */
Camera::Camera(float resolution_x, float resolution_y) {
  set_resolution(resolution_x, resolution_y);

  // set standart zoom (sensor_size)
  set_sensor_size(1, 1);
}

/***** Change Camera Parameters *****/

/**
 * @brief Change camera resolution.
 * 
 * @param resolution_x amount of pixels in x direction.
 * @param resolution_y amount of pixels in y direction.
 */
void Camera::set_resolution(float resolution_x, float resolution_y) {
  _resolution = vec2(resolution_x, resolution_y);
  _aspect_ratio = resolution_x / resolution_y;
}

/**
 * @brief Change camera resoultion (square image).
 * 
 * @param resolution_x amount of pixels in x and y direction.
 */
void Camera::set_resolution(float resolution) {
  _resolution = vec2(resolution, resolution);
  _aspect_ratio = 1;
}

void Camera::set_sensor_size(float x, float y) {
  _sensor_size = vec2(x, y);

  _pixel_size = vec2(
    _sensor_size.x / _resolution.x,
    _sensor_size.y / _resolution.y);
}

/***** Geters *****/

vec2 Camera::get_resolution() {
  return _resolution;
}

/***** Camera Functions *****/

/**
 * @brief Generate Ray trough the middle of a pixel starting at the origin.
 * 
 * @param pixel {x, y} pixel for the ray direction.
 * @return Ray 
 */
Ray Camera::get_ray(vec2 pixel) {
  vec2 pos_image = pixel_to_image_pos(pixel, vec2(0.5, 0.5));
  vec3 direction = image_to_world(pos_image);
  Ray res = Ray(_origin, direction);

  return res;
}

Ray Camera::get_ray(vec2 pixel, vec2 relative_position, float random_range) {
  // TODO add randomized range
  vec2 pos_image = pixel_to_image_pos(pixel, relative_position);
  vec3 direction = image_to_world(pos_image);
  Ray res = Ray(_origin, direction);

  return res;
}

/***** Transform Coordinates *****/

/**
 * @brief Transform a pixel position to a virtual image position.
 * 
 * @param pixel vec3 to be transformed.
 * @return vec2 position on virtual image.
 */
vec2 Camera::pixel_to_image_pos(vec2 pixel, vec2 relative_pos) {
  // check if pixel is valid
  if (pixel.x < 0 ||
      pixel.y < 0 ||
      pixel.x >= _resolution.x ||
      pixel.y >= _resolution.y) {
    throw std::invalid_argument("pixel not in given resolution");
  }

  vec2 res = vec2(
    relative_pos.x * _pixel_size.x + _pixel_size.x * pixel.x,
    relative_pos.y * _pixel_size.y + _pixel_size.y * pixel.y);

  return res;
}

/**
 * @brief Transform point from virtual image to world coordinates.
 * 
 * @param pos_image position in image.
 * @return vec3
 */
vec3 Camera::image_to_world(vec2 pos_image) {
  vec3 start = vec3(
    -0.5 * _sensor_size.x,
    -0.5 * _sensor_size.y,
    -1.f);

  vec3 p = vec3(pos_image.x, pos_image.y, 0.f);

  return start + p;
}

/***** Transformations *****/

void Camera::move(vec3 a) {
  _transform.add_translation(a);
}

/**
 * @brief returns inverse of camera transformation.
 * 
 * @return Transformation 
 */
Transformation Camera::get_view_transform(void) {
  Transformation t = {
      _transform.get_combined_inv(),
      _transform.get_combined()};
  return t;
}
void Camera::rotate(vec3 axis, float degree) {
  _transform.add_rotation(axis, degree);
}

void Camera::rotate(vec3 point, vec3 axis, float degree) {
  _transform.add_rotation(point, axis, degree);
}
