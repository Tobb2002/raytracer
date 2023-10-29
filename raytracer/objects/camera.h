/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include "ray.h"

using glm::vec3;
using glm::vec2;

class Camera {
 public:
  Camera(float resolution_x, float resolution_y);
  Ray get_ray(vec2 pixel);

  // getters
  vec2 get_resolution();
 private:
  const vec3 _origin = vec3(0, 0, 0);
  const vec3 _direction = vec3(0, 0, -1);
  vec2 _resolution;
  vec2 _image_size;
  vec2 _pixel_size;
  float _aspect_ratio;

  vec2 pixel_to_image_pos(vec2 pixel);
  vec3 image_to_world(vec2 pos_image);
};

#endif  // CAMERA_H_
