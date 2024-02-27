/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "image.hpp"
#include "scene.hpp"

#include "scenes/performance.hpp"


using glm::vec3, glm::vec2;

int main(void) {
  Scene scene = get_scene();

  Image out = scene.trace_image();
  char filename[50];
  sprintf(filename, "data/output/out.ppm");
  out.write_to_file(filename);

  return 0;
}
