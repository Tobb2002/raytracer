/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "../object_factory.hpp"
#include "../objects/mesh.hpp"
#include "../objects/plane.hpp"
#include "../objects/sphere.hpp"
#include "../scene.hpp"

#define TEXTURE true
Scene get_scene() {
  Scene scene = Scene(vec3(102, 255, 102));

  vec3 origin_plane = vec3(0, -0.7, -2);

#if TEXTURE
  Mesh m = Mesh("data/input/kingshall.obj", origin_plane + vec3(0, 0, -18),
                {.color = vec3(0.2, 0.2, 0.2), .specular = 0.0},
                "data/input/textures/kingshall.png");
#else
  Mesh m = Mesh("data/input/kingshall.obj", origin_plane + vec3(0, 0, -18),
                {.color = vec3(0.2, 0.2, 0.2), .specular = 0.0});
#endif

  Pointlight light = Pointlight(origin_plane + vec3(-3, 2, 4), 450);
  scene.add_light(light);
  // ObjectFactory factory = ObjectFactory(&scene);
  // factory.new_xy_square_light(origin_plane + vec3(-3, 2, 4), 300, 2, 0.05);

  scene.get_camera()->set_resolution(800, 600);
  scene.get_camera()->set_sensor_size(1.6, 1.2);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  scene.add_object(m);
  // scene.add_light(light1);

  return scene;
}

void animation_step(Scene *scene) {
  scene->get_camera()->rotate(vec3(0, 0, -5), vec3(0, 1, 0), 5);
  scene->update_view_transform();
}
