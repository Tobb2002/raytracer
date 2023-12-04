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
#include "objects/material.hpp"

#include "objects/bvh.hpp"


using glm::vec3;

int main(void) {
  vec3 origin = vec3(-2, -4, -13);
  Mesh m = Mesh("data/input/bunny_scaled.obj",
      vec3(1.2, -4.8, -10), {.color = vec3(0, 1, 0)});
  Mesh m2 = Mesh("data/input/cube.obj",
      vec3(-2, -3, -13), {.color = vec3(1, 0, 0), .mirror = 0.3});
  Plane plane = Plane(origin, vec3(0, 1, 0),
      {.color = vec3(1 , 1, 1), .mirror = 0.0},
      {.color = vec3(0.6, 0.6, 0.6), .mirror = 0.0});
  // Plane plane2 = Plane(vec3(0, 0, -50), vec3(0, 0, -1), vec3(0, 0.5, 1));
  Pointlight light = Pointlight(vec3(-2, -2, -16), 100);
  Pointlight light1 = Pointlight(vec3(6, 3, 1), 250);
  Pointlight light2 = Pointlight(vec3(-1.8, 2, 0), 100);

  Scene scene = Scene(vec3(0, 100, 200));

  scene.get_camera()->set_resolution(1000);
  scene.get_camera()->set_sensor_size(1, 1);

  // scene.add_light(&light);
  scene.add_light(&light1);
  // scene.add_light(&light2);
  scene.add_object(&m);
  scene.add_object(&plane);
  //scene.add_object(&m2);
  // scene.add_object(&plane2);

  // test bvh parts


  // generate animation
  float rotation = 90;
  float samples = 1;

  for (int i = 0; i < samples; i++) {
    std::cout << "Sample (" << i + 1 << " of " << samples << ")\n";
    Image out = scene.trace_image();
    char filename[50];
    sprintf(filename, "data/output/animation/sample%d.ppm", i);
    out.write_to_file(filename);

    // motion
    // plane.rotate(vec3(0, 1, 0), rotation/samples);

    //plane.rotate(vec3(1, 0, 0), rotation/samples);
    //scene.get_object(1)->rotate(vec3(1, 0, 0), rotation/samples);
    //m2.rotate(vec3(1, 0, 0), rotation/samples);
    //m2.move(vec3(0.1, 0, 0));
    scene.get_camera()->rotate(origin, vec3(0, 1, 0), rotation/samples);
    // scene.get_camera()->move(vec3(0, 0, -1));
    scene.update_view_transform();
    //scene.get_camera()->rotate(vec3(0, 1, 0), +5);
    //scene.update_view_transform();
    //scene.update_view_transform();
    //plane.move(vec3(1, 0, 0));

  }
  return 0;
}
