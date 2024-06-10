/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "../object_factory.hpp"
#include "../objects/plane.hpp"
#include "../objects/sphere.hpp"

Scene get_scene() {
  Scene scene = Scene(vec3(0, 50, 100));

  scene.get_camera()->set_resolution(800, 400);
  scene.get_camera()->set_sensor_size(1, 0.5);

  scene.set_aliasing(4);
  scene.set_tonemapping_value(-1);

  vec3 origin = vec3(0, 0, -13);

  Plane plane = Plane(
      origin + vec3(0, 0, -10), vec3(0, 0, -1),
      {.color = vec3(1, 1, 1), .diffuse = 0, .ambient = 0.6, .specular = 0},
      vec2(14, 10), "data/input/textures/planets/background_stars.png");

  Sphere earth = Sphere(origin, 2, {.specular = 0.2},
                        "data/input/textures/planets/earth_uv.png");
  earth.rotate(vec3(0, 1, 0), -20);

  Pointlight light1 = Pointlight(origin + vec3(-6, 2, 4), 320);

  scene.add_light(light1);

  scene.add_object(earth);
  scene.add_object(plane);

  return scene;
}
