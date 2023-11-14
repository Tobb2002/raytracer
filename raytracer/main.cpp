/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "objects/ray.hpp"
#include "objects/triangle.hpp"
#include "objects/camera.hpp"
#include "image.hpp"
#include "objects/pointlight.hpp"
#include "objects/mesh.hpp"
#include "objects/box.hpp"
#include "objects/plane.hpp"
#include "scene.hpp"


using glm::vec3;

void trace_object(Object *object);

int main(void) {
  std::string input_file = "data/input/bunny.obj";

  // Load .obj File
  // define objects for the scene
  Mesh m = Mesh(input_file, vec3(0, -0.1, -2));
  Plane plane = Plane(vec3(0, -1, 0), vec3(0, 1, 0), vec3(1, 0, 0));
  Pointlight light = Pointlight(vec3(0, 0, -2), vec3(255, 255, 255));

  Scene scene = Scene();

  scene.get_camera()->set_resolution(100, 100);
  scene.get_camera()->set_sensor_size(5, 5);

  scene.add_light(&light);
  scene.add_object(&plane);
  scene.add_object(&m);

  Image img = scene.trace_image();

  img.write_to_file("data/output/mesh.ppm");


  return 0;
}
