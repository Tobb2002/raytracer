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
  Mesh m = Mesh("data/input/bunny_scaled.obj", vec3(1.2, -4.8, -10));
  Mesh m2 = Mesh("data/input/test copy.obj", vec3(-4, -4, -17));
  Plane plane = Plane(vec3(0, -4, 0), vec3(0, 1, 0), vec3(0, 1, 0));
  Plane plane2 = Plane(vec3(0, 0, -50), vec3(0, 0, -1), vec3(0, 0.5, 1));
  Pointlight light = Pointlight(vec3(5, 2, 0), 300);
  Pointlight light2 = Pointlight(vec3(-1.8, 2, 0), 100);

  Scene scene = Scene();

  scene.get_camera()->set_resolution(100, 100);
  scene.get_camera()->set_sensor_size(1, 1);

  scene.add_light(&light);
  scene.add_light(&light2);
  scene.add_object(&m);
  scene.add_object(&m2);
  scene.add_object(&plane);
  scene.add_object(&plane2);

  Image img = scene.trace_image();

  img.write_to_file("data/output/mesh.ppm");

  return 0;
}
