/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#ifndef RAY_H_
#define RAY_H_

#include <glm/glm.hpp>

using glm::vec3;


class Ray {
 public:
    Ray(vec3 origin, vec3 direction);
   // getter
   vec3 get_origin();
   vec3 get_direction();

    vec3 get_point(float t);
 private:
    vec3 _origin;
    vec3 _direction;
};
#endif  // RAY_H_
