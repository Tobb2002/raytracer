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
 * HLBVH: TREELETBITS: 23
 *        GRID_SIZE: 13
 *
 * VISUALIZE_RANGE:
 */

inline Scene get_scene() {
  Scene scene = Scene(vec3(255));

  vec3 origin = vec3(0, 0, 0);
  vec3 camera_pos = vec3(-60000, 280000, 370000);

  Mesh m = Mesh("data/input/powerplant", "powerplant.obj", origin - camera_pos,
                {.color = vec3(0.9, 0.2, 0.2),
                 .ambient = vec3(0.15),
                 .specular = vec3(0.0)},
                AHLBVH);

  scene.add_object(Plane(origin - camera_pos, vec3(0, 1, 0),
                         {.color = vec3(0.2)}, {.color = vec3(0.8)}));

  ObjectFactory factory = ObjectFactory(&scene);
  // factory.new_xy_square_light(origin + vec3(-200000, 10, 100000), 480, 4,
  // 100);

  // Pointlight light = Pointlight(origin + vec3(1000000, 100, 10), 170);
  // scene.add_light(light);
  scene.add_light(Pointlight(origin + vec3(100000, 1000, 10000), 480));
  // scene.add_light(Pointlight(origin + vec3(0, 10000000, -30000), 150));
  // scene.add_light(Pointlight(origin + vec3(-1000000, 10, 100000), 150));

  scene.get_camera()->set_resolution(1200, 800);
  scene.get_camera()->set_sensor_size(1.5, 1);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  scene.add_object(m);
  // scene.add_light(light1);

  // adjust camera y and z are switched in blender
  // scene.get_camera()->move(vec3(-14, 10, -12));
  scene.get_camera()->rotate(vec3(1, 0, 0),
                             -30);  // blender rotation x offset -90
  // scene.get_camera()->rotate(vec3(0,0,1), 0); // blender: -y rotation
  // scene.get_camera()->rotate(vec3(0, 1, 0), -80);  // blender rotation z
  //
  scene.update_view_transform();
  return scene;
}

inline void animation_step(Scene *scene) {
  // scene->get_camera()->rotate(vec3(0, 0, -5), vec3(0, 1, 0), 5);
  scene->get_obj_mesh(0)->rotate(vec3(0, -0.7, -2), vec3(0, 1, 0), 10);
  scene->update_view_transform();
}
