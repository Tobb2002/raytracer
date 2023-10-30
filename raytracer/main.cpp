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


using glm::vec3;

void trace_triangle(void);
void trace_mesh(Mesh m);

int main(void) {
  std::string input_file = "data/input/test copy.obj";

  // Load .obj File
  Mesh m = Mesh(input_file, vec3(-1, -1, -1));

  trace_mesh(m);

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

void trace_triangle(Triangle* triangle,
                    Camera* camera,
                    Image* image,
                    Pointlight* pointlight) {
  vec2 resolution = camera->get_resolution();
  // iterate throug image and calulate intersection
  // between camera rays and Triangle
  for (int x = 0; x < resolution[0]; x++) {
    for (int y = 0; y < resolution[1]; y++) {
      Ray ray = camera->get_ray(vec2(x, y));

      float t = triangle->intersect(ray);
      if (t > 0) {
        // calculate reflected light at intersection point
        vec3 intersection = ray.get_point(t);

        vec3 color = calculate_phong(intersection,
                                     triangle->get_color(),
                                     triangle->get_normal(),
                                     pointlight);

        image->set_pixel({x, y}, color);
      }
    }
  }
}

void trace_mesh(Mesh mesh) {
  int resolution[2] = {200, 200};
  Camera camera = Camera(resolution[0], resolution[1]);

  Image image = Image(resolution[0], resolution[1]);

  Pointlight light_source = Pointlight(vec3(1, -1, 0), vec3(200, 200, 200));

  for (int x = 0; x < resolution[0]; x++) {
    for (int y = 0; y < resolution[1]; y++) {
      Intersection intersect = mesh.get_closest_intersection(camera.get_ray({x, y}));

      // if intersection found calculate color
      if (intersect.found) {
        vec3 color = calculate_phong(intersect.point, intersect.triangle.get_color(), intersect.triangle.get_normal(), &light_source);

        image.set_pixel({x, y}, color);
      }
    }
  }

  image.write_to_file("data/output/mesh.ppm");
}
