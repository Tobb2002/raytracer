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

#include "scenes/material_properties.hpp"


using glm::vec3, glm::vec2;

int main(void) {
  //Scene scene = get_scene();

  vec3 origin = vec3(-6, 0, 0);
  vec3 origin_plane = vec3(0, -2, -18);
  //Mesh m = Mesh("data/input/skull.obj",
  //    vec3(0, 15, -20), {.color = vec3(0, 1, 0)});


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

  for (int i = 0; i < samples; i++) {
    // for aliasing

 
    Image out = scene.trace_image();
    char filename[50];
    sprintf(filename, "data/output/animation/sample%d.ppm", i);
    out.write_to_file(filename);

    // motion

  }
  return 0;
}
