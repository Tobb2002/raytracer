/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "../object_factory.hpp"
#include "../objects/mesh.hpp"
#include "../objects/plane.hpp"
#include "../objects/sphere.hpp"
#include "../scene.hpp"

inline Scene get_scene() {
  Scene scene = Scene(vec3(102, 255, 102));

  vec3 origin_plane = vec3(0.7, -1.8, -2);

  Mesh m = Mesh("data/input", "dragon.obj", origin_plane + vec3(0, -0.1, 0),
                {.color = vec3(0.8, 0.2, 0.2), .specular = vec3(0.1)}, AHLBVH);

  ObjectFactory factory = ObjectFactory(&scene);
  factory.new_xy_square_light(origin_plane + vec3(-1.5, 2, 2), 370, 6, 0.02);

  scene.get_camera()->set_resolution(800 * 1.77, 800);
  scene.get_camera()->set_sensor_size(0.8 * 1.77, 0.8);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  scene.add_object(m);
  scene.add_object(Plane(origin_plane, vec3(0, 1, 0), {.color = vec3(0.2)},
                         {.color = vec3(0.8)}));
  // scene.add_light(light1);

  // adjust camera y and z are switched in blender
  // scene.get_camera()->move(vec3(-14, 10, -12));
  scene.get_camera()->rotate(vec3(1, 0, 0),
                             -35);  // blender rotation x offset -90
  // scene.get_camera()->rotate(vec3(0,0,1), 0); // blender: -y rotation
  scene.get_camera()->rotate(vec3(0, 1, 0), -15);  // blender rotation z
  //
  scene.update_view_transform();

  return scene;
}

inline void animation_step(Scene *scene) {
  // scene->get_camera()->rotate(vec3(0, 0, -5), vec3(0, 1, 0), 5);
  scene->get_obj_mesh(0)->rotate(vec3(0, -0.7, -2), vec3(0, 1, 0), 10);
  scene->update_view_transform();
}
