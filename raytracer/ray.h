/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#ifndef RAY_H_
#define RAY_H_

#include <glm/glm.hpp>

using namespace glm;


class Ray {
  public:
    Ray(vec3 origin, vec3 direction);
    vec3 get_point(float t);
  private:
    vec3 origin;
    vec3 direction;

};
#endif