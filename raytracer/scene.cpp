/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "scene.hpp"

#include <glm/gtx/string_cast.hpp>

// initialize Scene
Scene::Scene(vec3 standart_color) {
  _standart_light = standart_color;
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
                     Material material,
                     vec3 surface_normal,
                     Ray camera_ray) {
  vec3 res_color = vec3(0, 0, 0);

  surface_normal = glm::normalize(surface_normal);

  vec3 v = camera_ray.get_direction();
  
  // calculate light for all lightsources
  for (Pointlight *light : _lights) {
    vec3 incoming_light = light->get_color();

    Ray ray_to_light = Ray(point, light->get_light_direction(point));
    ray_to_light.move_into_dir(0.01);

    vec3 light_direction = ray_to_light.get_direction();

    float ndotl = glm::dot(surface_normal, light_direction);

    if (ndotl < 0) {
      surface_normal *= -1;
      ndotl = glm::dot(surface_normal, ray_to_light.get_direction());
    }

    vec3 r = 2 * ndotl * surface_normal - light_direction;

    float rdotv = glm::dot(r, v);

    vec3 l_material = vec3(0, 0, 0);

    vec3 l_ambient = material.color * material.ambient;
    vec3 l_diffuse = material.diffuse * material.color;
    
    vec3 l_specular = vec3(0, 0, 0);
    if (rdotv > 0) {
      l_specular = material.specular * (vec3(1, 1, 1) * glm::pow(rdotv, material.pow_m));
    }
    else {
      rdotv *= -1;
      l_specular = material.specular * (vec3(1, 1, 1) * glm::pow(rdotv, material.pow_m));
    }
    // only ad the ones which are not blocked
    if (!check_intersection(ray_to_light, light->get_distance(point))) {
      l_material = l_diffuse + l_specular;
    }

    vec3 l_cam = incoming_light * ndotl * l_material + incoming_light * l_ambient;

    // perfect mirror
    if (material.mirror > 0) {
      vec3 color = get_color(generate_reflection_ray(point, surface_normal, v));
      if (color.x == -1) {
        color = vec3(0, 0, 0);
      }
      l_cam = (l_cam * (1 - material.mirror)) + (color * material.mirror);
    }
    res_color += vec3(glm::round(l_cam.x),
                      glm::round(l_cam.y),
                      glm::round(l_cam.z));
  }

  return res_color;
}

Ray Scene::generate_reflection_ray(vec3 point,
                                   vec3 normal,
                                   vec3 viewer_direction) {
  viewer_direction *= -1;
  float vdotn = glm::dot(viewer_direction, normal);
  if (vdotn < 0) {
  std::cout << vdotn << "\n";
  }
  vec3 dir = 2 * vdotn * normal - viewer_direction;

  Ray res = Ray(point, dir);
  res.move_into_dir(0.01);
  return res;
}

vec3 Scene::get_color(Ray ray) {
  // calculate object intersections
  Material material;
  Intersection best_intersection = {false,
                                    MAXFLOAT,
                                    vec3(0, 0, 0),
                                    vec3(0, 0, 0),
                                    material};
  
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
    vec3 light = calculate_phong(best_intersection.point,
                    best_intersection.material,
                    best_intersection.normal,
                    ray);

    return light;
  }
  else {
    // didn't hit any object
    return vec3(-1);
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
      vec3 color = get_color(_camera.get_ray({x, y}));
      if (color.x == -1){
        color = _standart_light;
      }
      image.set_pixel({x, y}, color);
    }
  }
  return image;
}
