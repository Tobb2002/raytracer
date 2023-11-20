/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "scene.hpp"

#include <glm/gtx/string_cast.hpp>

// initialize Scene
Scene::Scene(vec3 standart_color) {
  _standart_color = standart_color;
}

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
  for (Object *object : _objects) {
    if (object->intersect_bool(ray, t_max)) {
      return true;
    }
  }
  return false;
}

// rendering functions
vec3 Scene::calculate_phong(vec3 point,
                     vec3 material,
                     vec3 surface_normal,
                     Ray camera_ray) {
  vec3 res_color = vec3(0, 0, 0);

  surface_normal = glm::normalize(surface_normal);

  float spec_factor = 1.4;
  float diffuse_factor = 0.7;
  float ambient_factor = 0.2;

  float pow_m = 5;

  // calculate light for all lightsources
  for (Pointlight *light : _lights) {
    vec3 incoming_light = light->get_color();

    Ray ray_to_light = Ray(point, light->get_light_direction(point));
    ray_to_light.move_into_dir(0.01);

    vec3 light_direction = ray_to_light.get_direction();
    vec3 v = camera_ray.get_direction();

    float ndotl = glm::dot(surface_normal, light_direction);

    if (ndotl < 0) {
      surface_normal *= -1;
      ndotl = glm::dot(surface_normal, ray_to_light.get_direction());
    }

    vec3 r = 2 * ndotl * (surface_normal - light_direction);

    float rdotv = glm::dot(r, v);

    vec3 l_material;

    vec3 l_ambient = material * ambient_factor;
    vec3 l_diffuse = diffuse_factor * material;
    
    vec3 l_specular = vec3(0, 0, 0);
    if (rdotv > 0) {
      l_specular = spec_factor * (vec3(1, 1, 1) * glm::pow(rdotv, pow_m));
    }
    else {
      rdotv *= -1;
      l_specular = spec_factor * (vec3(1, 1, 1) * glm::pow(rdotv, pow_m));
    }
    // only ad the ones which are not blocked
    if (!check_intersection(ray_to_light, light->get_distance(point))) {
      l_material = l_ambient + l_diffuse + l_specular;
    }
    else {
      l_material = l_ambient;
    }

    vec3 l_cam = incoming_light * ndotl * l_material;
    res_color += vec3(glm::round(l_cam.x),
                      glm::round(l_cam.y),
                      glm::round(l_cam.z));
  }

  return res_color;
}

vec3 Scene::get_color(Ray ray) {
  // calculate object intersections
  Intersection best_intersection = {false,
                                    MAXFLOAT,
                                    vec3(0, 0, 0),
                                    vec3(0, 0, 0),
                                    vec3(0, 0, 0)};
  
  // find closest intersection in Scene
  for (Object *object : _objects) {
    Intersection intersect = object->intersect(ray);

    if (intersect.found) {
      if (intersect.t < best_intersection.t) {
        best_intersection = intersect;
      }
    }
  }
  if (best_intersection.found && best_intersection.t > 0) {
    vec3 color = calculate_phong(best_intersection.point,
                    best_intersection.color,
                    best_intersection.normal,
                    ray);

    return color;
  }
  else {
    return _standart_color;
  }
}

Image Scene::trace_image() {
  Image image = Image(_camera.get_resolution().x,
                      _camera.get_resolution().y);

  int resolution[2] =
    {static_cast<int>(_camera.get_resolution().x),
    static_cast<int>(_camera.get_resolution().y)};



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

      // get color from ray
      image.set_pixel({x, y}, get_color(_camera.get_ray({x, y})));
    }
  }
  return image;
}
