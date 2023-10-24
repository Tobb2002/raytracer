/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <iostream>
#include "./filehandler.h"
#include "./vector.h"
#include <glm/glm.hpp>
#include "./ray.h"
#include <glm/gtx/string_cast.hpp>

using glm::vec3;


int main(void) {
  safe_picture("filename.ppm");

  return 0;
}
