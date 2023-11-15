/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <vector>

#include "objects/object.hpp"
#include "objects/pointlight.hpp"
#include "objects/camera.hpp"
#include "image.hpp"

class Scene {
 public:
  Scene();

  // add objects
  size_t add_light(Pointlight *light);
  size_t add_object(Object *object);

  // get objects to manipulate them
  Camera *get_camera(void);
  Object *get_object(size_t id);


  // rendering functions
  Image trace_image();

 private:
  std::vector<Pointlight*> _lights;
  std::vector<Object*> _objects;
  Camera _camera;

  vec3 calculate_phong(vec3 point,
                       vec3 material,
                       vec3 surface_normal);
  bool check_intersection(Ray ray, float t_max);
};
