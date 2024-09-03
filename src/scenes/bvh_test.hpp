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

  vec3 origin_plane = vec3(0, 0, 0);

  Mesh m = Mesh("data/input/bvh_test.obj", origin_plane + vec3(0, 0, 0),
                {.color = vec3(0.2, 0.2, 0.2), .specular = 0.2}, true);

  Plane plane =
      Plane(origin_plane, vec3(0, 1, 0),
            {.color = vec3(1, 1, 1), .specular = 0, .mirror = 0.0},
            {.color = vec3(0.6, 0.6, 0.6), .specular = 0, .mirror = 0.0},
            vec2(100, 15));

  Pointlight light1 = Pointlight(origin_plane + vec3(-3, 2, 4), 250);

  scene.get_camera()->set_resolution(500);
  scene.get_camera()->set_sensor_size(1, 1);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  scene.add_object(m);
  scene.add_light(light1);

  return scene;
}

inline void animation_step(Scene *scene) {
  scene->rotate_obj_mesh(0, vec3(0, 0, 1), 30);
}
