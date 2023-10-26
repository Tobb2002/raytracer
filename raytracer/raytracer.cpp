/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "./ray.h"
#include "./triangle.h"
#include "./camera.h"
#include "image.h"

using glm::vec3;


int main(void) {
  // try to trace first image
  // setup scene
  vec3 points[] = {vec3(-3, -4, -1), vec3(4, -3, -1), vec3(0, 4, -1) };
  Triangle t = Triangle(points, vec3(200, 0, 0));

  int resolution[2] = {100, 100};
  Camera camera = Camera(resolution[0], resolution[1]);

  Image image = Image(resolution[0], resolution[1]);

  // iterate throug image and calulate intersection
  // between camera rays and Triangle
  for (int x = 0; x < resolution[0]; x++) {
    for (int y = 0; y < resolution[1]; y++) {
      Ray ray = camera.get_ray(vec2(x, y));

      if (t.intersect_bool(ray)) {
        image.set_pixel({x, y}, t.get_color());
      }
    }
  }

  image.write_to_file("test.ppm");


  return 0;
}
