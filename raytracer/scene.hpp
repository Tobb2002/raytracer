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
  Scene(vec3 standart_color);

  // add objects
  size_t add_light(Pointlight *light);
  size_t add_object(Object *object);

  // get objects to manipulate them
  Camera *get_camera(void);
  Object *get_object(size_t id);

  vec3 get_color(Ray ray);


  // rendering functions
  Image trace_image();

 private:
  std::vector<Pointlight*> _lights;
  std::vector<Object*> _objects;
  Camera _camera;

  vec3 _standart_light;

  vec3 calculate_phong(vec3 point,
                       Material material,
                       vec3 surface_normal,
                       Ray camera_ray);
  bool check_intersection(Ray ray, float t_max);
};
