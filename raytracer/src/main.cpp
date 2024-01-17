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
#include "objects/sphere.hpp"


using glm::vec3;


int main(void) {
  vec3 origin = vec3(0, -4, -13);
  //Mesh m = Mesh("data/input/skull.obj",
  //    vec3(0, 15, -20), {.color = vec3(0, 1, 0)});
  Mesh m = Mesh("data/input/bunny_scaled.obj",
      vec3(1.2, -4.8, -10), {.color = vec3(0, 1, 0)});
  Sphere s = Sphere(origin + vec3(-3, 2, 0), 1,{.color = vec3(1, 0, 1)});
  //Mesh c1 = Mesh("data/input/cube.obj",
  //    vec3(-2, -3, -13), {.color = vec3(1, 0, 0), .mirror = 0.3});
  Mesh c2 = Mesh("data/input/cube.obj",
      vec3(0, -3, -13), {.color = vec3(1, 0, 0), .mirror = 0.});
  Mesh c3 = Mesh("data/input/cube.obj",
      vec3(-2, 0, -13), {.color = vec3(1, 0, 0), .mirror = 0.});
  //Mesh c4 = Mesh("data/input/cube.obj",
  //    vec3(+2, 0, -13), {.color = vec3(1, 0, 0), .mirror = 0.});
  //Mesh c5 = Mesh("data/input/cube.obj",
  //    vec3(-2, -3, -17), {.color = vec3(1, 0, 0), .mirror = 0.});
  //Mesh c6 = Mesh("data/input/cube.obj",
  //    vec3(+2, -3, -17), {.color = vec3(1, 0, 0), .mirror = 0.});
  //Mesh c7 = Mesh("data/input/cube.obj",
  //    vec3(-2, 0, -17), {.color = vec3(1, 0, 0), .mirror = 0.});
  //Mesh c8 = Mesh("data/input/cube.obj",
  //    vec3(+2, 0, -17), {.color = vec3(1, 0, 0), .mirror = 0.3});
  Plane plane = Plane(origin, vec3(0, 1, 0),
      {.color = vec3(1 , 1, 1), .mirror = 0.0},
      {.color = vec3(0.6, 0.6, 0.6), .mirror = 0.0});
  // Plane plane2 = Plane(vec3(0, 0, -50), vec3(0, 0, -1), vec3(0, 0.5, 1));
  Pointlight light = Pointlight(vec3(-2, -2, -16), 100);
  Pointlight light1 = Pointlight(vec3(8, 8, -3), 100);
  Pointlight light2 = Pointlight(vec3(-6, 8, 1), 150);

  Scene scene = Scene(vec3(0, 100, 200));

  std::cout << "Scene size:" << m.get_size() << std::endl;

 
  scene.get_camera()->set_resolution(500);
  scene.get_camera()->set_sensor_size(1, 1);

  scene.set_aliasing(2);
  scene.set_tonemapping_value(4);

  // scene.add_light(&light);
  //scene.add_light(&light2);
  scene.add_light(&light1);
  scene.add_light(&light2);
  //scene.add_object(&c2);
  scene.add_object(&s);
  scene.add_object(&c3);
  //scene.add_object(&m);
  s.print();
  //scene.add_object(&c3);
  scene.add_object(&plane);
  //scene.add_object(&m2);
  // scene.add_object(&plane2);

  // test bvh parts


  // generate animation
  float rotation = 90;
  float samples = 1;

  for (int i = 0; i < samples; i++) {
    // for aliasing
    scene.get_camera()->rotate(origin, vec3(0, 1, 0), 20);
    scene.update_view_transform();

    std::cout << "Sample (" << i + 1 << " of " << samples << ")\n";
    Image out = scene.trace_image();
    char filename[50];
    sprintf(filename, "data/output/animation/sample%d.ppm", i);
    out.write_to_file(filename);

    // motion
    // plane.rotate(vec3(0, 1, 0), rotation/samples);

    //plane.rotate(vec3(1, 0, 0), rotation/samples);
    //scene.get_object(1)->rotate(vec3(1, 0, 0), rotation/samples);
    //c1.rotate(vec3(0, 1, 0), rotation/samples * 2);
    //c8.rotate(vec3(0, 1, 0), rotation/samples * 2);
    //m.move(vec3(0.1, 0, 0));
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
