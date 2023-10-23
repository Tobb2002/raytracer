/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "filehandler.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct rgb_pixel {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};


void init_picture(void) {
  int size_x = 100;
  int size_y = 100;

  rgb_pixel bild[size_x][size_y];

  cout << sizeof(bild);

}

void safe_picture(string filename) {
  /** 
   * Function that outputs given Picture in ppm format.
   * Parameters:
   *  picture: <type>
   *  location: <string>
   */
  int sizex = 100;
  int sizey = 100;

  // initialise File
  ofstream file(filename);
  
  // File header
  file << "P3\n" << sizex << " " << sizey << "\n255\n";

  // Picture Content
  rgb_pixel p1 = {0,100,200};

  for (int x = 0; x < sizex; x++) {
    for (int y = 0; y < sizey; y++) {
      
      file << (int)p1.red << " " << (int)p1.green << " " << (int)p1.blue << "\n";
    }
  }

}
