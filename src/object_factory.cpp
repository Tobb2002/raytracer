/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "object_factory.hpp"

ObjectFactory::ObjectFactory(Scene *scene) {
  _scene = scene;
}

// order amount^2 objects in a square given by size, middle position and normal.
void ObjectFactory::new_square_pattern(Object *object,
                                       int amount,
                                       float size,
                                       vec3 position,
                                       vec3 normal) {
  // place all objects in rectangle around origin
  float step_size =  size / amount;

  // Object **objects = new Object[size][size];
  for (int x = 0; x < amount; x++) {
    for (int y = 0; y < amount; y++) {
     // _scene->add_object(object);
    }
  }
  // apply_transform all objects to final position
}

void ObjectFactory::new_xy_square_light(
    const vec3 &position,
    const int &strength,
    const uint &amount,
    const float &space) {
  // square light
  float ssize = 0.3;

  for (int x = 0; x < amount ; x++) {
    for (int y = 0; y < amount; y++) {
      Pointlight l =
        Pointlight(position + vec3(x * ssize, y * ssize, 0),
                   static_cast<float>(strength) /
                   (amount * amount));
      _scene->add_light(l);
    }
  }
}
