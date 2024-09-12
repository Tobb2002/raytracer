/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "../object_factory.hpp"
#include "../objects/mesh.hpp"
#include "../objects/plane.hpp"
#include "../objects/sphere.hpp"
#include "../scene.hpp"

inline Scene get_scene() {
  Scene scene = Scene(vec3(0));

  vec3 origin = vec3(-6, -4, -10);

  vec3 tree_pos = vec3(0, -14, -28);

  Mesh house = Mesh("data/input/coffee_house2.obj", origin,
                    {.color = vec3(0.32, 0.21, 0.01), .specular = 0.0});

  Pointlight light = Pointlight(vec3(0, 10, 0), 350);
  // scene.add_light(light);
  Pointlight light1 = Pointlight(vec3(13, 15, 0), 550);
  scene.add_light(light1);
  // ObjectFactory factory = ObjectFactory(&scene);
  // factory.new_xy_square_light(vec3(6, 8, -15), 300, 4, 0.1);

  scene.get_camera()->set_resolution(800, 600);
  scene.get_camera()->set_sensor_size(1.6, 1.2);

  scene.set_aliasing(5);
  scene.set_tonemapping_value(-1);

  scene.add_object(house);

  scene.get_obj_mesh(0)->rotate(origin, vec3(0, 1, 0), 20);
  scene.update_view_transform();
  // scene.add_light(light1);

  return scene;
}

inline void animation_step(Scene *scene) {
  scene->get_camera()->rotate(vec3(0, 0, -5), vec3(0, 1, 0), -20);
  scene->update_view_transform();
}
