/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "./filehandler.h"
#include "./ray.h"
#include "./triangle.h"

using glm::vec3;


int main(void) {
  safe_picture("filename.ppm");

  vec3 points[] = {vec3(0,0,0), vec3(1,2,3), vec3(1,1,1) };

  Triangle t = Triangle(points, vec3(100, 100, 100));
  t.print();

  return 0;
}
