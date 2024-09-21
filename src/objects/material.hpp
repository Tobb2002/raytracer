/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include <string>

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
  vec3 ambient = vec3(0.25);

  /// @brief factor for specular material.
  vec3 specular = vec3(0.7);
  float pow_m = 2;

  /// @brief factor for mirroring material.
  float mirror = 0;

  // textures
  std::string diffuse_texname = "";             // map_Kd
};
