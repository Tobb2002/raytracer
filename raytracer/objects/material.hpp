/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>

using glm::vec3;

// Material propities with standart values.
struct Material {
  vec3 color = vec3(100, 100, 100);

  float diffuse = 0.7;
  float ambient = 0.25;
  float specular = 1.4;
  float pow_m = 5;

  float mirror = 0; 
};