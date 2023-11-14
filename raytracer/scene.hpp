/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once;

#include <vector>

#include "objects/object.hpp"
#include "objects/pointlight.hpp"
#include "objects/camera.hpp"
#include "image.hpp"

class Scene {
 public:
  Scene();

  void add_light(Pointlight light);
  // adds an object and return it id.
  size_t add_object(Object object);

  Object *get_object(int id);

  Image render_image();

 private:
  std::vector<Pointlight> _lights;
  std::vector<Object> _objects;
  Camera _camera;
};