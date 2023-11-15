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

bool Scene::check_intersection(Ray ray, float t_max) {
  for (Object *object: _objects) {
    if (object->intersect_bool(ray, t_max)) {
      return true;
    }
  }
  return false;
}

// rendering functions
vec3 Scene::calculate_phong(vec3 point,
                     vec3 material,
                     vec3 surface_normal) {
  vec3 res_color = vec3(0, 0, 0);

  // find reachable light sources
  Pointlight *light = _lights.at(0);

  vec3 col = light->get_color();
  vec3 s = vec3(material.x * col.x, material.y * col.y, material.z * col.z);

  Ray r_to_light = Ray(point, light->get_light_direction(point));
  r_to_light.move_into_dir(0.01);

  if (!check_intersection(r_to_light, light->get_distance(point))) {
    float nl = glm::dot(surface_normal, r_to_light.get_direction());

    if (nl < 0) {
      surface_normal *= -1;
    }
    nl = glm::dot(surface_normal, r_to_light.get_direction());


    vec3 v = s * nl;
    res_color += vec3(glm::round(v.x), glm::round(v.y), glm::round(v.z));
    // todo divide by number of light sources
  }

  return res_color;
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
      Intersection best_intersection = {false,
                                        MAXFLOAT,
                                        vec3(0, 0, 0),
                                        vec3(0, 0, 0),
                                        vec3(0, 0, 0)};
      for (Object *object: _objects) {
        Intersection intersect = object->intersect(
                                        _camera.get_ray({x, y}));

        // if intersection found calculate color
        if (intersect.found) {
          if (intersect.t < best_intersection.t) {
            best_intersection = intersect;
          }
        }
      }
      if (best_intersection.found && best_intersection.t > 0) {
        vec3 color = calculate_phong(best_intersection.point,
                        best_intersection.color,
                        best_intersection.normal);

        image.set_pixel({x, y}, color);
      }
    }
  }
  return image;
}