/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "../objects/plane.hpp"
#include "../objects/sphere.hpp"
#include "../scene.hpp"

inline Scene get_scene() {
  Scene scene = Scene(vec3(0, 50, 100));

  vec3 origin = vec3(0, -2, -18);

  Sphere earth = Sphere(origin + vec3(7, 0, 0), 1,
                        {.color = vec3(1, 0, 1), .specular = 0.2},
                        "data/input/earth_uv.png");
  Sphere venus = Sphere(origin + vec3(4, 0, 0), 1.1,
                        {.color = vec3(1, 0, 1), .specular = 0.2},
                        "data/input/textures/planets/venus.jpg");
  Sphere mars = Sphere(origin + vec3(9, 0, 0), 0.5,
                       {.color = vec3(1, 0, 1), .specular = 0.2},
                       "data/input/textures/planets/mars.jpg");
  Sphere sun = Sphere(origin + vec3(-16, 0, 0), 15,
                      {.color = vec3(1, 0, 1), .specular = 0.2},
                      "data/input/textures/planets/sonne.jpg");
  return scene;
}
