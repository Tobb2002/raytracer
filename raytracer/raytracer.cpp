/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <iostream>
#include "./filehandler.h"
#include "./vector.h"


int main(void) {
  safe_picture("filename.ppm");

  init_picture();

  Vector v = Vector(1,2,3);
  
  v.print();

  return 0;
}
