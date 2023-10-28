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

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "mesh.h"


using glm::vec3;

void trace_triangle(void);
void trace_mesh(Mesh m);

int main(void) {
  
  // test obj loader
  //test_obj();


  std::string input_file = "input/test.obj";

	// Load .obj File
  Mesh m = Mesh(input_file);

  trace_mesh(m);

  return 0;
}


void trace_triangle(Triangle triangle, Camera* camera, Image* image) {

  vec2 resolution = camera->get_resolution();
  // iterate throug image and calulate intersection
  // between camera rays and Triangle
  for (int x = 0; x < resolution[0]; x++) {
    for (int y = 0; y < resolution[1]; y++) {
      Ray ray = camera->get_ray(vec2(x, y));

      if (triangle.intersect_bool(ray)) {
        image->set_pixel({x, y}, triangle.get_color());
      }
    }
  }


}
void trace_mesh(Mesh mesh) {
  int resolution[2] = {100, 100};
  Camera camera = Camera(resolution[0], resolution[1]);

  Image image = Image(resolution[0], resolution[1]);


  for (int i = 0; i < mesh.get_size(); i++) {
    trace_triangle(mesh.get_triangle(i), &camera, &image);
    mesh.get_triangle(i).print();

  }
  image.write_to_file("output/mesh.ppm");
}
