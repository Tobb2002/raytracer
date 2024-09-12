/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "../objects/plane.hpp"
#include "../objects/sphere.hpp"
#include "../scene.hpp"

inline Scene get_scene() {
  Scene scene = Scene(vec3(255));

  vec3 origin = vec3(0, 0, -6);

  scene.get_camera()->set_resolution(800, 600);
  scene.get_camera()->set_sensor_size(1.6, 1.2);

  scene.set_aliasing(5);
  scene.set_tonemapping_value(-1);

  Pointlight light1 = Pointlight(vec3(3, 4, 0), 300);
  scene.add_light(light1);

  Sphere s = Sphere(origin, 2, {.color = vec3(1, 0, 0), .specular = 0.4});

  scene.add_object(s);
  return scene;
}
