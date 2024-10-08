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
 *
 * VISUALIZE_RANGE: 100, 1200
 */

inline Scene get_scene() {
  Scene scene = Scene(vec3(102, 255, 102));

  vec3 origin = vec3(0, 0, 0);
  vec3 camera_pos = vec3(-18, 9, 0);

  Mesh m = Mesh("data/input/bistro", "bistro.obj", origin - camera_pos,
                {.color = vec3(0.2, 0.2, 0.2),
                 .ambient = vec3(0.25),
                 .specular = vec3(0.15)},
                AHLBVH);

  vec3 light = vec3(1, 0.9, 0.9);
  ObjectFactory factory = ObjectFactory(&scene);
  factory.new_xy_square_light(origin + vec3(10, 40, -3), light * vec3(70), 5,
                              0.55);
  factory.new_xy_square_light(origin + vec3(-350, 380, -140), light * vec3(500),
                              5, 10.2);

  // Pointlight light = Pointlight(origin + vec3(0, 0, 0), 450);
  // scene.add_light(light);

  scene.get_camera()->set_resolution(800 * 1.77, 800);
  scene.get_camera()->set_sensor_size(1.77, 1);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  scene.add_object(m);
  // scene.add_light(light1);

  // adjust camera y and z are switched in blender
  // scene.get_camera()->move(vec3(-14, 10, -12));
  scene.get_camera()->rotate(vec3(1, 0, 0),
                             -15);  // blender rotation x offset -90
  // scene.get_camera()->rotate(vec3(0,0,1), 0); // blender: -y rotation
  scene.get_camera()->rotate(vec3(0, 1, 0), -80);  // blender rotation z
  //
  scene.update_view_transform();
  return scene;
}

inline void animation_step(Scene *scene) {
  // scene->get_camera()->rotate(vec3(0, 0, -5), vec3(0, 1, 0), 5);
  scene->get_obj_mesh(0)->rotate(vec3(0, -0.7, -2), vec3(0, 1, 0), 10);
  scene->update_view_transform();
}
