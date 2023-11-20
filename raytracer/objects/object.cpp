/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "object.hpp"


Intersection Object::intersect(Ray ray) {
  return {false, 0, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0)};
}
bool Object::intersect_bool(Ray ray, float t_max) {
  Intersection i = intersect(ray);

  if (i.found && i.t < t_max) {
    return true;
  }

  return false;
}

void Object::print(void) {
  std::cout << "-----Object-----\n";
}

// transformation functions
void Object::rotate(void) {

}