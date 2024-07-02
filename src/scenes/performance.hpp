/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "../object_factory.hpp"
#include "../objects/mesh.hpp"
#include "../objects/plane.hpp"
#include "../objects/sphere.hpp"
#include "../scene.hpp"

Scene get_scene() {
  Scene scene = Scene(vec3(102, 255, 102));

  vec3 origin_plane = vec3(0, -0.7, -2);

  // Mesh m = Mesh("data/input/dragon.obj", origin_plane + vec3(0, 0, 0),
  //               {.color = vec3(0.2, 0.2, 0.2), .specular = 0.2});

  Mesh m = Mesh("data/input/kingshall.obj", origin_plane + vec3(0, 0, -18),
                {.color = vec3(0.2, 0.2, 0.2), .specular = 0.2},
                "data/input/textures/kingshall.png");

  Plane plane =
      Plane(origin_plane, vec3(0, 1, 0),
            {.color = vec3(1, 1, 1), .specular = 0, .mirror = 0.0},
            {.color = vec3(0.6, 0.6, 0.6), .specular = 0, .mirror = 0.0},
            vec2(100, 15));

  Pointlight light1 = Pointlight(origin_plane + vec3(-3, 2, 4), 450);
  // ObjectFactory factory = ObjectFactory(&scene);
  // factory.new_xy_square_light(origin_plane + vec3(-3, 2, 4), 400, 1, 0.1);

  scene.get_camera()->set_resolution(500);
  scene.get_camera()->set_sensor_size(1, 1);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  scene.add_object(m);
  scene.add_light(light1);

  return scene;
}

Mesh get_m() {
  vec3 origin_plane = vec3(0, -0.7, -2);
  Mesh m = Mesh("data/input/dragon.obj", origin_plane + vec3(0, 0, 0),
                {.color = vec3(0.2, 0.2, 0.2), .specular = 0.2});

  return m;
}
