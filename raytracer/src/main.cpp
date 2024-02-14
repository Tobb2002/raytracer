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
#include "objects/texture.hpp"


using glm::vec3, glm::vec2;


int main(void) {
  
  Texture texture;
    //Texture("data/input/earth_uv.png");
  texture.load_image("data/input/earth_uv.png");

  //texture.show_image();

  vec3 colors = texture.get_color_uv(vec2(0.77,0.77));
  std::cout << "Farben" << glm::to_string(colors) << "\n";


  vec3 origin = vec3(-6, 0, 0);
  vec3 origin_plane = vec3(0, 0, -18);
  //Mesh m = Mesh("data/input/skull.obj",
  //    vec3(0, 15, -20), {.color = vec3(0, 1, 0)});
  Mesh m = Mesh("data/input/Airplane.obj",
      origin_plane + vec3(0, 0, 0), {.color = vec3(0, 1, 0), .specular=0.2});
  Sphere earth = Sphere(origin + vec3(7, 0, 0), 1,{.color = vec3(1, 0, 1), .specular=0.2}, "data/input/earth_uv.png");
  Sphere venus = Sphere(origin + vec3(4, 0, 0), 1.1,{.color = vec3(1, 0, 1), .specular=0.2}, "data/input/textures/planets/venus.jpg");
  Sphere mars = Sphere(origin + vec3(9, 0, 0), 0.5,{.color = vec3(1, 0, 1), .specular=0.2}, "data/input/textures/planets/mars.jpg");
  Sphere sun = Sphere(origin + vec3(-16, 0, 0), 15,{.color = vec3(1, 0, 1), .specular=0.2}, "data/input/textures/planets/sonne.jpg");
  //Mesh c1 = Mesh("data/input/cube.obj",
  //    origin_plane + vec3(0, 0, 0), {.color = vec3(1, 0, 0), .mirror = 0.3});
  //Mesh c2 = Mesh("data/input/cube.obj",
  //    origin + vec3(2, +1, -0.2), {.color = vec3(1, 0, 0), .mirror = 0.});
  //Mesh c3 = Mesh("data/input/cube.obj",
  //    vec3(-2, 0, -13), {.color = vec3(1, 0, 0), .mirror = 0.});
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
  Plane plane = Plane(origin_plane, vec3(0, 1, 0),
      {.color = vec3(1 , 1, 1), .mirror = 0.3},
      {.color = vec3(0.6, 0.6, 0.6), .mirror = 0.3},
                      vec2(100,50));
  //plane.set_axis(true);
  // Plane plane2 = Plane(vec3(0, 0, -50), vec3(0, 0, -1), vec3(0, 0.5, 1));
  Pointlight light = Pointlight(origin + vec3(0, 4, 0), 300);
  Pointlight light1 = Pointlight(origin_plane + vec3(-3, 5, 4), 250);
  Pointlight light2 = Pointlight(vec3(-6, 8, 1), 150);

  Scene scene = Scene(vec3(0, 50, 100));

  std::cout << "Scene size:" << m.get_size() << std::endl;


  scene.get_camera()->set_resolution(100);
  scene.get_camera()->set_sensor_size(1, 1);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  scene.add_light(&light1);
  //scene.add_light(&light);

  //scene.add_object(&sun);
  //scene.add_object(&earth);
  //scene.add_object(&venus);
  //scene.add_object(&mars);
    //
  scene.add_object(&m); 
  //scene.add_object(&plane);
  //scene.add_object(&c1);

  // generate animation
  float rotation = 180;
  float samples = 1;

  //scene.get_camera()->rotate(origin, vec3(1, 0, 0), -12);
  scene.get_camera()->move(vec3(0, 7, 0));
  scene.get_camera()->rotate(origin, vec3(1, 0, 0), -15);
  scene.update_view_transform();

  for (int i = 0; i < samples; i++) {
    // for aliasing

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

  //  scene.get_camera()->rotate(origin, vec3(0, 1, 0), rotation/samples);

    //s.rotate(vec3(0, 1, 0), rotation/samples);
    //c2.rotate(vec3(0, 1, 0), rotation/samples);
    // scene.get_camera()->move(vec3(0, 0, -1));
 //   scene.update_view_transform();
    //scene.get_camera()->rotate(vec3(0, 1, 0), +5);
    //scene.update_view_transform();
    //scene.update_view_transform();
    //plane.move(vec3(1, 0, 0));

  }
  return 0;
}
