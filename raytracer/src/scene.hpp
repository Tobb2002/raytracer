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
  explicit Scene(vec3 standart_color);

  /***** Adding things to scene *****/

  size_t add_light(Pointlight *light);
  size_t add_object(Object *object);

  /***** Getters *****/

  Camera *get_camera(void);
  Object *get_object(size_t id);

  vec3 get_light(Ray ray);


  /***** Rendering *****/

  Image trace_image();

 private:
  std::vector<Pointlight*> _lights;
  std::vector<Object*> _objects;

  Camera _camera;

  /// @brief light if a ray hit's nothing
  vec3 _standart_light;

  Ray generate_reflection_ray(vec3 point, vec3 normal, vec3 viewer_direction);

  vec3 calculate_light(
      vec3 point,
      Material material,
      vec3 surface_normal,
      Ray camera_ray);
  vec3 get_phong(
      Pointlight *light,
      Material material,
      vec3 point,
      vec3 normal,
      vec3 viewing_direction);
  vec3 get_mirroring_light(
      Material material,
      vec3 point,
      vec3 normal,
      vec3 viewing_direction);
  bool check_intersection(Ray ray, float t_max);
};
