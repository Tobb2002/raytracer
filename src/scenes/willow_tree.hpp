/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "../object_factory.hpp"
#include "../objects/mesh.hpp"
#include "../objects/plane.hpp"
#include "../objects/sphere.hpp"
#include "../scene.hpp"

inline Scene get_scene() {
  Scene scene = Scene(vec3(255, 255, 255));

  vec3 origin = vec3(0, -0.7, -5);

  vec3 tree_pos = vec3(0, -14, -28);

  Mesh tree = Mesh("data/input/willow_tree_combined.obj", origin + tree_pos,
                   {.color = vec3(0.32, 0.21, 0.01), .specular = 0.0}, AHLBVH);

  // Pointlight light = Pointlight(origin + vec3(-3, 2, 4), 350);
  // scene.add_light(light);
  ObjectFactory factory = ObjectFactory(&scene);
  factory.new_xy_square_light(origin + vec3(-3, 2, 4), 300, 1, 0.1);

  scene.get_camera()->set_resolution(1600, 1200);
  scene.get_camera()->set_sensor_size(1.6, 1.2);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  scene.add_object(tree);

  return scene;
}

inline void animation_step(Scene *scene) {
  scene->get_camera()->rotate(vec3(0, 0, -5), vec3(0, 1, 0), 5);
  scene->update_view_transform();
}
