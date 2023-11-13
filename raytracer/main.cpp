/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "objects/ray.h"
#include "objects/triangle.h"
#include "objects/camera.h"
#include "image.h"
#include "objects/pointlight.h"
#include "objects/mesh.h"
#include "objects/box.h"
#include "objects/plane.hpp"


using glm::vec3;

void trace_triangle(void);
void trace_mesh(Mesh m, Plane plane);

void trace_object(Object *object);

int main(void) {
  std::string input_file = "data/input/bunny.obj";

  // Load .obj File
  Mesh m = Mesh(input_file, vec3(0, -0.1, -2));
  Plane plane = Plane(vec3(0, -1, 0), vec3(0, 1, 0), vec3(1, 0, 0));

  trace_object(&m);

  return 0;
}


vec3 calculate_phong(vec3 point,
                     vec3 material,
                     vec3 surface_normal,
                     Pointlight* light) {
  vec3 c = light->get_color();
  vec3 s = vec3(material.x * c.x, material.y * c.y, material.z * c.z);

  float nl = glm::dot(surface_normal, light->get_light_direction(point));

  if (nl < 0) {
    surface_normal *= -1;
  }
  nl = glm::dot(surface_normal, light->get_light_direction(point));


  vec3 v = s * nl;

  return vec3(glm::round(v.x), glm::round(v.y), glm::round(v.z));
}

void trace_object(Object *object) {
  int resolution[2] = {100, 100};
  Camera camera = Camera(resolution[0], resolution[1]);

  camera.set_sensor_size(0.1, 0.1);

  Image image = Image(resolution[0], resolution[1]);

  Pointlight light_source = Pointlight(vec3(0, 0, -2), vec3(255, 255, 255));

  float progress_step = 0.05 * resolution[0];
  float progress = 0;
  std::cout << "rendering\n\n";
  for (int x = 0; x < resolution[0]; x++) {
    for (int y = 0; y < resolution[1]; y++) {
      // print progress
      if (x > progress) {
        // erase line and write new progress
        std::cout << "\e[2K\e[1A" << "Progress: " << 100 * progress / resolution[0] << "%\n";
        progress += progress_step;
      }

      Intersection intersect = object->intersect(
                                      camera.get_ray({x, y}));

      // if intersection found calculate color
      if (intersect.found) {
        vec3 color = calculate_phong(intersect.point,
                        intersect.color,
                        intersect.normal,
                        &light_source);

        image.set_pixel({x, y}, color);
      }
      
    }
  }

  image.write_to_file("data/output/mesh.ppm");
}