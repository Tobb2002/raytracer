/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

// functions for transforming points
#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using glm::vec3, glm::vec4, glm::mat4x4;

// scale a given relative to the origin
vec3 scale(vec3 point, vec3 factor) {
  vec4 f = vec4(factor, 1);
  mat4x4 scale_mat = mat4x4(1.f);

  scale_mat[1] = f;

  return scale_mat * f;
}

#endif  // TRANSFORM_H_
