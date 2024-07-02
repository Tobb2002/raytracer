/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <glm/glm.hpp>

#include "image.hpp"
#include "scene.hpp"
#include "scenes/bvh_test.hpp"

using glm::vec3, glm::vec2;

// #define ANIMATION
#define FRAMES 1

// void animation_step(Scene *scene) {
//   scene->get_obj_mesh(0)->rotate(vec3(0,0,1), 10);
// }

int main(void) {
  Scene scene = get_scene();

#ifndef ANIMATION
  Image out = scene.trace_image();
  char filename[50];
  snprintf(filename, sizeof(filename), "data/output/out.ppm");
  out.write_to_file(filename);
#else
  for (size_t i = 0; i < 3; i++) {
    Image out = scene.trace_image();
    char filename[50];
    snprintf(filename, sizeof(filename), "data/output/animation/out%zu.ppm", i);
    out.write_to_file(filename);

    // animation_step(&scene);
  }
#endif

  return 0;
}
