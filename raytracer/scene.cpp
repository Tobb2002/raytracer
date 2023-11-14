/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "scene.hpp"

// initialize Scene
Scene::Scene() { }

size_t Scene::add_light(Pointlight *light) {
   _lights.push_back(light);
   return _lights.size() -1;
}

size_t Scene::add_object(Object *object) {
   _objects.push_back(object);
   return _objects.size() -1;
}


// get elements from scene
Camera *Scene::get_camera(void) {
  return &_camera;
}

Object *Scene::get_object(size_t id) {
  return _objects.at(id);
}


// rendering functions
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

Image Scene::trace_image() {
  Image image = Image(_camera.get_resolution().x,
                      _camera.get_resolution().y);

  int resolution[2] = {static_cast<int>(_camera.get_resolution().x), static_cast<int>(_camera.get_resolution().y)};



  float progress_step = 0.05 * resolution[0];
  float progress = 0;
  std::cout << "rendering\n\n";
  for (int x = 0; x < resolution[0]; x++) {
    for (int y = 0; y < resolution[1]; y++) {
      // print progress
      if (x > progress) {
        // erase line and write new progress
        std::cout << "\e[2K\e[1A" << "Progress: "
        << 100 * progress / resolution[0] << "%\n";
        progress += progress_step;
      }

      // calculate object intersections
      for (Object *object: _objects) {
        Intersection intersect = object->intersect(
                                        _camera.get_ray({x, y}));

        // if intersection found calculate color
        if (intersect.found) {
          vec3 color = calculate_phong(intersect.point,
                          intersect.color,
                          intersect.normal,
                          _lights.at(0));

          image.set_pixel({x, y}, color);
        }
      }
    }
  }
  return image;
}