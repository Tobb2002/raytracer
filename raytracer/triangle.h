/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <glm/glm.hpp>
#include "./ray.h"

using glm::vec3;

class Triangle {
 public:
    Triangle(vec3 points[3], vec3 color);

    vec3 intersect(Ray ray);
    bool intersect_bool(Ray ray);

    // getters
    vec3 get_color();

    void print();
 private:
    vec3 _p[3];
    vec3 _color;
};

#endif  // TRIANGLE_H_
