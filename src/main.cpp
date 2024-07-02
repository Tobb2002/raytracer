/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <glm/glm.hpp>

#include "image.hpp"
#include "scene.hpp"
#include "scenes/performance.hpp"

using glm::vec3, glm::vec2;

int main(void) {
  Scene scene = get_scene();
  //Scene scene = Scene(vec3(0, 50, 100));

  //vec3 origin_plane = vec3(0, 0, -13);

  //Mesh m = Mesh("data/input/bunny_fin.obj", origin_plane + vec3(-2, 0, 0),
  //              {.color = vec3(0, 1, 0), .specular = 0.2});
  //Mesh m_s =
  //    Mesh("data/input/bunny_fin_smooth.obj", origin_plane + vec3(2, 0, 0),
  //         {.color = vec3(0, 1, 0), .specular = 0.2});

  //Plane plane =
  //    Plane(origin_plane, vec3(0, 1, 0),
  //          {.color = vec3(1, 1, 1), .specular = 0, .mirror = 0.0},
  //          {.color = vec3(0.6, 0.6, 0.6), .specular = 0, .mirror = 0.0},
  //          vec2(100, 15));

  //ObjectFactory factory = ObjectFactory(&scene);
  //factory.new_xy_square_light(origin_plane + vec3(-5, 6, 5), 250, 4, 0.4);

  //scene.get_camera()->set_resolution(800, 400);
  //scene.get_camera()->set_sensor_size(1, 0.5);

  //scene.set_aliasing(4);
  //scene.set_tonemapping_value(-1);

  //scene.add_object(m);
  //scene.add_object(m_s);
  //scene.add_object(plane);
  


  //Scene scene = Scene(vec3(102, 255, 102));

  //vec3 origin_plane = vec3(0, -0.7, -2);

  ////Mesh m1 = Mesh("data/input/dragon.obj", origin_plane + vec3(0, 0, 0),
  ////              {.color = vec3(0.2, 0.2, 0.2), .specular = 0.2});

  //Mesh m = Mesh("data/input/kingshall.obj", origin_plane + vec3(0, 0, -18),
  //              {.color = vec3(0.2, 0.2, 0.2), .specular = 0.2},
  //              "data/input/textures/kingshall.png");

  //Mesh m1 = get_m();
  ////Mesh m2 = get_m();

  //Plane plane =
  //    Plane(origin_plane, vec3(0, 1, 0),
  //          {.color = vec3(1, 1, 1), .specular = 0, .mirror = 0.0},
  //          {.color = vec3(0.6, 0.6, 0.6), .specular = 0, .mirror = 0.0},
  //          vec2(100, 15));

  //Pointlight light1 = Pointlight(origin_plane + vec3(-3, 2, 4), 450);
  //// ObjectFactory factory = ObjectFactory(&scene);
  //// factory.new_xy_square_light(origin_plane + vec3(-3, 2, 4), 400, 1, 0.1);

  //scene.get_camera()->set_resolution(500);
  //scene.get_camera()->set_sensor_size(1, 1);

  //scene.set_aliasing(1);
  //scene.set_tonemapping_value(-1);

  //scene.add_object(m);
  //scene.add_light(light1);

  Image out = scene.trace_image();
  char filename[50];
  snprintf(filename, sizeof(filename), "data/output/out.ppm");
  out.write_to_file(filename);

  return 0;
}
