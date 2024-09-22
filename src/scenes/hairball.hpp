/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "../object_factory.hpp"
#include "../objects/mesh.hpp"
#include "../objects/plane.hpp"
#include "../objects/sphere.hpp"
#include "../scene.hpp"

/*
 * Gute Werte für die Datenstrukturen:
 * HLBVH: TREELETBITS: 25
 */

inline Scene get_scene() {
  Scene scene = Scene(vec3(255, 255, 255));

  vec3 origin = vec3(0, 0, -2);

  Mesh m = Mesh("data/input/hairball", "hairball.obj", origin + vec3(0, 0, -8),
                {.color = vec3(0.2, 0.2, 0.2),
                 .ambient = vec3(0.3),
                 .specular = vec3(0.15)},
                AHLBVH);

  ObjectFactory factory = ObjectFactory(&scene);
  factory.new_xy_square_light(origin + vec3(0.8, 2, -3), 300, 1, 0.05);
  // factory.new_xy_square_light(origin + vec3(-0.8, 2, -3), 300, 3, 0.05);

  // Pointlight light = Pointlight(origin + vec3(0, 0, 0), 450);
  // scene.add_light(light);

  scene.get_camera()->set_resolution(800, 600);
  scene.get_camera()->set_sensor_size(1.6, 1.2);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  scene.add_object(m);
  // scene.add_light(light1);

  return scene;
}

inline void animation_step(Scene *scene) {
  // scene->get_camera()->rotate(vec3(0, 0, -5), vec3(0, 1, 0), 5);
  scene->get_obj_mesh(0)->rotate(vec3(0, -0.7, -2), vec3(0, 1, 0), 10);
  scene->update_view_transform();
}
