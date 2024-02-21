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

#include "scenes/smooth_shading.hpp"


using glm::vec3, glm::vec2;

Scene test() {
  Scene scene = Scene(vec3(0, 50, 100));

  vec3 origin = vec3(-6, 0, 0);
  vec3 origin_plane = vec3(0, -2, -18);
  //Mesh m = Mesh("data/input/skull.obj",
  //    vec3(0, 15, -20), {.color = vec3(0, 1, 0)});
  Mesh m = Mesh("data/input/bunny_smooth.obj",
      origin_plane + vec3(0, -0.8, 0), {.color = vec3(0, 1, 0), .specular=0.2});

  //scene.add_object(m);

  return scene;
}



int main(void) {
  //Scene scene = get_scene();

  vec3 origin = vec3(-6, 0, 0);
  vec3 origin_plane = vec3(0, -2, -18);
  //Mesh m = Mesh("data/input/skull.obj",
  //    vec3(0, 15, -20), {.color = vec3(0, 1, 0)});
  Mesh m = Mesh("data/input/bunny_smooth.obj",
      origin_plane + vec3(0, -0.8, 0), {.color = vec3(0, 1, 0), .specular=0.2});


  Scene scene = get_scene();


  // square light
  uint amount = 5;
  float ssize = 0.05;
  float strength = 100;

  //std::vector<Pointlight> square_light;
  //square_light.reserve(amount * amount);
  //vec3 orig = origin_plane + vec3(-3, 4, 5);
  //for (int x = 0; x < amount ; x++) {
  //  for (int y = 0; y < amount; y++) {
  //    Pointlight l = Pointlight(orig + vec3(x * ssize, y * ssize, 0), strength / amount);
  //    square_light.push_back(l);
  //    scene.add_light(&square_light.at(y));
  //  }
  //}
  //scene.add_object(&c1);

  // generate animation
  float rotation = 180;
  float samples = 1;

  //scene.get_camera()->rotate(origin, vec3(1, 0, 0), -12);
  //scene.get_camera()->move(vec3(0, 7, 0));
  //scene.get_camera()->rotate(origin, vec3(1, 0, 0), -15);
  //scene.update_view_transform();

  for (int i = 0; i < samples; i++) {
    // for aliasing

 
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
