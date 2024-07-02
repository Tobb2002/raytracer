/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <glm/glm.hpp>
#include <memory>

#include "image.hpp"
#include "scene.hpp"
#include "scenes/performance.hpp"

using glm::vec3, glm::vec2;

// #define ANIMATION
#define FRAMES 1

Image get_image() { return Image(100, 100); }

int main(void) {
  Scene scene = get_scene();

#ifndef ANIMATION
  Image out = scene.trace_image();
  char filename[50];
  snprintf(filename, sizeof(filename), "data/output/out.ppm");
  out.write_to_file(filename);
#else
  Image out = scene.trace_image();
  for (size_t i = 0; i < 3; i++) {
    animation_step(&scene);
    char filename[50];
    snprintf(filename, sizeof(filename), "data/output/animation/out%zu.ppm", i);
    out.write_to_file(filename);

    out = scene.trace_image();
  }
#endif

  return 0;
}
