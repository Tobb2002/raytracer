/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "./filehandler.h"
#include "./ray.h"
#include "./triangle.h"
#include "./camera.h"
#include "image.h"

using glm::vec3;


int main(void) {
  safe_picture("filename.ppm");

  vec3 points[] = {vec3(2, 0, 0), vec3(2, 4, 0), vec3(2, 2, 2) };

  Ray r = Ray(vec3(0, 2, 0), vec3(1, 0, 0));

  Triangle t = Triangle(points, vec3(100, 100, 100));

  Camera camera = Camera(2, 2);

  Ray ray = camera.get_ray(vec2(0, 0));
  
  ray.print();

  // test making and writing image
  Image img = Image(100, 100);
  img.write_to_file("test.ppm");

  return 0;
}
