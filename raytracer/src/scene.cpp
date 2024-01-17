/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "scene.hpp"

#include <glm/gtx/string_cast.hpp>

/**
 * @brief Construct a new Scene:: Scene object
 * 
 * @param standart_color Color of a pixel that retrieves
 * no light from the scene.
 */
Scene::Scene(vec3 standart_color) {
  _standart_light = standart_color;
  set_aliasing(1);
}

/**
 * @brief Add a Pointlight to the scene.
 * 
 * @param light
 * @return size_t id of light.
 */
size_t Scene::add_light(Pointlight *light) {
  _lights.push_back(light);
  return _lights.size() -1;
}

/**
 * @brief Add a Object to scene.
 * 
 * @param object
 * @return size_t id of Object.
 */
size_t Scene::add_object(Object *object) {
  _objects.push_back(object);
  return _objects.size() -1;
}

/**
 * @brief set number of rays to calculate per pixel
 * 
 * @param rays_per_pixel default: 1 possible values: (1, 2, 4)
 */
void Scene::set_aliasing(uint rays_per_pixel) {
  _aliasing_positions.clear();
  switch(rays_per_pixel) {
    case 1:
      _aliasing_positions.push_back(vec2(0.5, 0.5));
      break;
    case 2:
      _aliasing_positions.push_back(vec2(0.25, 0.25));
      _aliasing_positions.push_back(vec2(0.75, 0.75));
      break;
    case 4:
      _aliasing_positions.push_back(vec2(0.25, 0.25));
      _aliasing_positions.push_back(vec2(0.25, 0.75));
      _aliasing_positions.push_back(vec2(0.75, 0.25));
      _aliasing_positions.push_back(vec2(0.75, 0.75));
      break;
    case 5:
      _aliasing_positions.push_back(vec2(0.25, 0.25));
      _aliasing_positions.push_back(vec2(0.25, 0.75));
      _aliasing_positions.push_back(vec2(0.75, 0.25));
      _aliasing_positions.push_back(vec2(0.75, 0.75));
      _aliasing_positions.push_back(vec2(0, 0));
      break;
    default:
      throw std::runtime_error("No Valid value for aliasing.");
      break;
  }
}

/**
 * @brief Get pointer to the camera in the scene.
 * 
 * @return Camera* 
 */
Camera *Scene::get_camera(void) {
  return &_camera;
}

/**
 * @brief Get pointer to object in the scene.
 * 
 * @param id id of the Object
 * @return Object* 
 */
Object *Scene::get_object(size_t id) {
  return _objects.at(id);
}

/**
 * @brief Check if a Ray intersects with an Object in the Scene.
 * 
 * @param ray
 * @param t_max maximum length of ray to check.
 * @return true if intersection found
 * @return false else
 */
bool Scene::check_intersection(Ray ray, float t_max) {
  for (Object *object : _objects) {
    if (object->intersect_bool(ray, t_max)) {
      return true;
    }
  }
  return false;
}

/***** Functions for Rendering *****/

/**
 * @brief Render an Image of the Scene.
 * 
 * @return Image rendered image.
 */
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
      vec3 color = vec3(0, 0, 0);
      for (int i = 0; i < _aliasing_positions.size(); i++) {
        vec3 light = get_light(_camera.get_ray({x, y}, _aliasing_positions.at(i), 0.2));
          
        if (light.x == -1) {
          light = _standart_light;
        }
        light *= 1.f /_aliasing_positions.size();
        color += light;
      }
      image.set_pixel({x, y}, color);
    }
  }
  return image;
}

/**
 * @brief Caluclate phong wiht diffuse, specular and ambient light.
 * 
 * @param light Lightsource to get light from.
 * @param material
 * @param point Point where light get's reflected.
 * @param normal surface normal at given point.
 * @param viewing_direction 
 * @return vec3 
 */
vec3 Scene::get_phong(
    Pointlight *light,
    Material material,
    vec3 point,
    vec3 normal,
    vec3 viewing_direction) {
  vec3 incoming_light = light->get_color();

  Ray ray_to_light = Ray(point, light->get_light_direction(point));
  ray_to_light.move_into_dir(0.01);

  vec3 light_direction = ray_to_light.get_direction();

  float ndotl = glm::dot(normal, light_direction);

  if (ndotl < 0) {
    normal *= -1;
    ndotl = glm::dot(normal, ray_to_light.get_direction());
  }

  vec3 r = 2 * ndotl * normal - light_direction;

  float rdotv = glm::dot(r, viewing_direction);

  vec3 l_material = vec3(0, 0, 0);

  vec3 l_ambient = material.color * material.ambient;
  vec3 l_diffuse = material.diffuse * material.color;

  vec3 l_specular = vec3(0, 0, 0);
  if (rdotv > 0) {
    l_specular = material.specular *
        (vec3(1, 1, 1) * glm::pow(rdotv, material.pow_m));
  } else {
    rdotv *= -1;
    l_specular = material.specular * (vec3(1, 1, 1) *
        glm::pow(rdotv, material.pow_m));
  }
  // only ad the ones which are not blocked
  if (!check_intersection(ray_to_light, light->get_distance(point))) {
    l_material = l_diffuse + l_specular;
  }

  return incoming_light * ndotl *
      l_material + incoming_light * l_ambient;
}

/**
 * @brief Calculate the light reflection if material was perfect mirror.
 * 
 * @param material  returns nothing if material.mirror = 0.
 * @param point point on which light get's reflected.
 * @param normal normal of reflecting surface
 * @param viewing_direction direction of the viewer.
 * @return vec3
 */
vec3 Scene::get_mirroring_light(
    Material material,
    vec3 point,
    vec3 normal,
    vec3 viewing_direction) {
  // perfect mirror
  vec3 color = vec3(0, 0, 0);
  if (material.mirror > 0) {
    color = get_light(
        generate_reflection_ray(point, normal, viewing_direction));
    if (color.x == -1) {
      color = vec3(0, 0, 0);
    }
  }
  return color;
}

/**
 * @brief Calculate light reflected from a surface.
 * 
 * @param point From which light is preceved
 * @param material Material of the surface at the given point.
 * @param surface_normal Normal of the surface.
 * @param ray Ray to which the light get's transported.
 * @return vec3 amount of light transported in direction of ray.
 */
vec3 Scene::calculate_light(
    vec3 point,
    Material material,
    vec3 surface_normal,
    Ray ray) {

  vec3 res_light = vec3(0, 0, 0);

  surface_normal = glm::normalize(surface_normal);
  vec3 v = ray.get_direction();

  // calculate light for all lightsources
  for (Pointlight *light : _lights) {
    vec3 phong_light = get_phong(
        light, material, point, surface_normal, v);
    vec3 mirror_light = get_mirroring_light(
        material, point, surface_normal, v);

    res_light += (phong_light * (1 - material.mirror)) +
        (mirror_light * material.mirror);
  }

  return glm::round(res_light);
}

/**
 * @brief Generate a ray into a reflection direction.
 * 
 * @param point Point where the light get's reflected.
 * @param normal Normal of the reflecting surface.
 * @param viewer_direction direction to which the
 * light get's reflected.
 * @return Ray 
 */
Ray Scene::generate_reflection_ray(
    vec3 point,
    vec3 normal,
    vec3 viewer_direction) {
  viewer_direction *= -1;
  float vdotn = glm::dot(viewer_direction, normal);
  vec3 dir = 2 * vdotn * normal - viewer_direction;

  Ray res = Ray(point, dir);
  res.move_into_dir(0.01);
  return res;
}

void Scene::update_view_transform(void) {
  Transformation view_transform = _camera.get_view_transform();
  for (Object * object: _objects) {
    object->update_view_transform(view_transform);
  }
  for (Pointlight *light: _lights) {
    light->update_view_transform(view_transform);
  }
}

/// @brief Calculates light transport a long a ray in the scene.
/// @param ray
/// @return amount of light reflected to direction of ray.
vec3 Scene::get_light(Ray ray) {
  // calculate object intersections
  Material material;
  Intersection best_intersection = {
      false,
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
  if (best_intersection.found) {
    vec3 light = calculate_light(
        best_intersection.point,
        best_intersection.material,
        best_intersection.normal,
        ray);

    return light;
  }
  // didn't hit any object
  return vec3(-1);
}
