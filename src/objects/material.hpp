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
  vec3 color = vec3(1, 1, 1);

  /// @brief factor for diffuse material
  vec3 diffuse = vec3(0.7);

  /// @brief factor to define the amount of ambient light to add.
  vec3 ambient = vec3(0.2);

  /// @brief factor for specular material.
  vec3 specular = vec3(0.0);
  float pow_m = 0;

  /// @brief factor for mirroring material.
  float mirror = 0;

  // textures
  int texture_id_diffuse = 0;
  int texture_id_specular = 0;
};
