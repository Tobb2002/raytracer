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
  float step_size =  size / (amount -1);
  float x = - size / 2;

  // Object **objects = new Object[size][size];
  for (int x = 0; x < amount; x++) {
    for (int y = 0; y < amount; y++) {
      // objects = _scene->add_object(object);
    }
  }
  // transform all objects to final position
}
