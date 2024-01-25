/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>

using glm::vec3;

// Material propities with standart values.
/**
 * @brief Definition of a Material.
 * 
 * Material::diffuse
 * 
 */
struct Material {
  /// @brief color of the Material.
  vec3 color = vec3(100, 100, 100);

  /// @brief factor for diffuse material
  float diffuse = 0.7;

  /// @brief factor to define the amount of ambient light to add.
  float ambient = 0.25;

  /// @brief factor for specular material.
  float specular = 0.7;
  float pow_m = 2;

  /// @brief factor for mirroring material.
  float mirror = 0;
};
