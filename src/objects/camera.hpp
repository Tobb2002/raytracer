/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <glm/glm.hpp>
#include "ray.hpp"
#include "transform.hpp"

using glm::vec3;
using glm::vec2;

class Camera {
 public:
  Camera();
  Camera(float resolution_x, float resolution_y);

  // getters
  vec2 get_resolution();

  Ray get_ray(vec2 pixel);
  Ray get_ray(vec2 pixel, vec2 relative_position, float random_range);

  // setters
  void set_sensor_size(float x, float y);
  void set_resolution(float resolution_x, float resolution_y);
  void set_resolution(float resolution_xy);

  /***** Transformations *****/
  void rotate(vec3 axis, float degree);
  void rotate(vec3 point, vec3 axis, float degree);
  void move(vec3 a);

  Transformation get_view_transform(void);

 private:
  const vec3 _origin = vec3(0, 0, 0);
  const vec3 _direction = vec3(0, 0, -1);
  vec2 _resolution;
  vec2 _sensor_size;
  vec2 _pixel_size;
  float _aspect_ratio;

  vec2 pixel_to_image_pos(vec2 pixel, vec2 relative_pos);
  vec3 image_to_world(vec2 pos_image);

  /***** Transformation *****/

  Transform _transform;
};
