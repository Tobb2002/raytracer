/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#pragma once

#include <glm/glm.hpp>
#include "object.hpp"
#include "light.hpp"

using glm::vec3;

class Pointlight : public Light {
 public:
  Pointlight(vec3 position, vec3 color);
  Pointlight(vec3 position, float intensity);

};
