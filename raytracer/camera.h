/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <glm/glm.hpp>

using glm::vec3;

class Camera {
 public:
  Camera(float resolution_x, float resolution_y);
  vec3 get_ray();
 private:
  const vec3 _origin = vec3(0, 0, 0);
  const vec3 _direction = vec3(0, 0, -1);
};