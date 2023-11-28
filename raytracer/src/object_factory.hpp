/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include "scene.hpp"
#include <glm/glm.hpp>

using glm::vec3;

class ObjectFactory {
  explicit ObjectFactory(Scene *scene);

  void new_square_pattern(
      Object *object,
      int amount,
      float size,
      vec3 position,
      vec3 normal);

 private:
  Scene *_scene;
};
