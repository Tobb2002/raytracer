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

  vec3 points[] = {vec3(2,0,0), vec3(2,4,0), vec3(2,2,2) };

  Ray r = Ray(vec3(0, 10, 0), vec3(0, 1, 0));

  Triangle t = Triangle(points, vec3(100, 100, 100));
  t.print();

  r.print();
  // test triangle ray intersection
  vec3 res = t.intersect(r);


  std::cout << glm::to_string(res) << std::endl;

  return 0;
}
