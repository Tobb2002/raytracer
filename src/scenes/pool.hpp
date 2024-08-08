/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "../object_factory.hpp"
#include "../objects/plane.hpp"
#include "../objects/sphere.hpp"
#include "../scene.hpp"

inline Scene get_scene() {
  Scene scene = Scene(vec3(0, 0, 0));

  vec3 origin = vec3(0, 0, -12);

  Plane plane =
      Plane(origin + vec3(0, -1, 0), vec3(0, 1, 0),
            {.color = vec3(0.2, 1, 0.2), .specular = 0, .mirror = 0.0},
            vec2(20, 30), "data/input/textures/pool/table.png");

  scene.get_camera()->set_resolution(800, 400);
  scene.get_camera()->set_sensor_size(1, 0.5);
  scene.set_aliasing(4);
  scene.set_tonemapping_value(-1);

  // --- add lights ---
  ObjectFactory factory = ObjectFactory(&scene);
  factory.new_xy_square_light(origin + vec3(-3, 4, 5), 250, 4, 0.3);

  // --- add objects ---
  scene.add_object(plane);

  uint count = 0;
  char texture[40];
  int seq[15] = {1, 9, 2, 3, 8, 10, 11, 7, 14, 4, 5, 13, 15, 6, 12};
  // textures gehen nicht: 8

  for (int i = 1; i <= 5; i++) {
    for (int j = 0; j < i; j++) {
      vec3 pos = origin + vec3(0, 0, 0) + vec3(j * 2 - i, 0, -1.73 * i);
      snprintf(texture, sizeof(texture), "data/input/textures/pool/ball%i.png",
               seq[count]);
      Sphere b = Sphere(
          pos, 1, {.color = vec3(1, 0, 1), .specular = 0.8, .mirror = 0.3},
          texture);
      // rotate ball randomly
      int rand = 359 * static_cast<double>(std::rand()) / RAND_MAX;
      b.rotate(vec3(rand, rand, rand), rand);

      scene.add_object(b);
      count++;
    }
  }

  scene.get_camera()->move(vec3(2, 3, 0));
  scene.get_camera()->rotate(origin, vec3(1, 0, 0), -35);
  scene.get_camera()->rotate(origin, vec3(0, 1, 0), 30);
  scene.update_view_transform();
  return scene;
}
