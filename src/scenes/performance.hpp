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

  Mesh m = Mesh("data/input/dragon.obj", origin_plane + vec3(0, 0, 0),
                {.color = vec3(0.2, 0.2, 0.2), .specular = 0.1});

  ObjectFactory factory = ObjectFactory(&scene);
  factory.new_xy_square_light(origin_plane + vec3(-3, 2, 4), 370, 2, 0.05);

  scene.get_camera()->set_resolution(800, 600);
  scene.get_camera()->set_sensor_size(1.6, 1.2);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  scene.add_object(m);
  // scene.add_light(light1);

  return scene;
}

void animation_step(Scene *scene) {
  // scene->get_camera()->rotate(vec3(0, 0, -5), vec3(0, 1, 0), 5);
  scene->get_obj_mesh(0)->rotate(vec3(0, -0.7, -2), vec3(0, 1, 0), 10);
  scene->update_view_transform();
}
