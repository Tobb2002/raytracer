/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <iostream>
#include <glm/glm.hpp>

#include "ray.hpp"

using glm::vec3;

struct Intersection {
  bool found;
  float t;
  vec3 point;
  vec3 normal;
  vec3 color;
};

class Object {
 public:
  virtual Intersection intersect(Ray ray) {
    return {false, 0, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0)};
  }
  virtual bool intersect_bool(Ray ray, float t_max) {
    Intersection i = intersect(ray);

    if (i.found && i.t < t_max) {
      return true;
    }

    return false;
  }

  virtual void print(void) {
    std::cout << "Empty Object\n";
  }

 private:
  vec3 _origin;
  vec3 _color;


};
