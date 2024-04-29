/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "object_factory.hpp"

ObjectFactory::ObjectFactory(Scene *scene) {
  _scene = scene;
}

void ObjectFactory::new_xy_square_light(
    const vec3 &position,
    const int &strength,
    const uint &amount,
    const float &space) {
  // square light
  float ssize = space;

  for (uint x = 0; x < amount ; x++) {
    for (uint y = 0; y < amount; y++) {
      Pointlight l =
        Pointlight(position + vec3(x * ssize, y * ssize, 0),
                   static_cast<float>(strength) /
                   (amount * amount));
      _scene->add_light(l);
    }
  }
}
