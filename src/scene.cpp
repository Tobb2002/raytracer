/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "scene.hpp"

#include <time.h>

#include <chrono>
#include <glm/gtx/string_cast.hpp>
#include <memory>

#include "objects/plane.hpp"

// #define DEBUG
// #define PRINT_PROGRESS

#define NO_SHADING

Scene::Scene() {
  _standart_light = vec3(0);
  set_aliasing(1);
}
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

Scene::Scene(const Scene &old_scene) {
  _lights = old_scene._lights;
  _obj_planes = old_scene._obj_planes;
  _obj_spheres = old_scene._obj_spheres;
  _obj_meshes = old_scene._obj_meshes;

  _camera = old_scene._camera;
  _standart_light = old_scene._standart_light;
  _tonemapping_gray = old_scene._tonemapping_gray;
  _aliasing_positions = old_scene._aliasing_positions;
}

Scene &Scene::operator=(const Scene &old_scene) {
  _lights = old_scene._lights;
  _obj_planes = old_scene._obj_planes;
  _obj_spheres = old_scene._obj_spheres;
  _obj_meshes = old_scene._obj_meshes;

  _camera = old_scene._camera;
  _standart_light = old_scene._standart_light;
  _tonemapping_gray = old_scene._tonemapping_gray;
  _aliasing_positions = old_scene._aliasing_positions;

  return *this;
}

/**
 * @brief Add a Pointlight to the scene.
 *
 * @param light
 * @return size_t id of light.
 */
size_t Scene::add_light(Pointlight light) {
  _lights.push_back(light);
  return _lights.size() - 1;
}

/**
 * @brief Add a plane to the scene.
 *
 * @param plane
 * @return size_t id of plane
 */
size_t Scene::add_object(Plane plane) {
  _obj_planes.push_back(plane);
  return _obj_planes.size() - 1;
}

/**
 * @brief Add a Sphere to the scene.
 *
 * @param sphere
 * @return size_t id of plane
 */
size_t Scene::add_object(Sphere sphere) {
  _obj_spheres.push_back(sphere);
  return _obj_spheres.size() - 1;
}

/**
 * @brief Add a Mesh to the scene.
 *
 * @param sphere
 * @return size_t id of mesh
 */
size_t Scene::add_object(Mesh mesh) {
  _obj_meshes.push_back(mesh);
  return _obj_meshes.size() - 1;
}

void Scene::rotate_obj_mesh(size_t id, vec3 axis, float degree) {
  _obj_meshes.at(id).rotate(axis, degree);
}
Mesh *Scene::get_obj_mesh(size_t id) {
  std::cout << "mesh size: " << _obj_meshes.size() << "\n";
  return &_obj_meshes.at(id);
}
/**
 * @brief set number of rays to calculate per pixel
 *
 * @param rays_per_pixel default: 1 possible values: (1, 2, 4)
 */
void Scene::set_aliasing(uint rays_per_pixel) {
  _aliasing_positions.clear();
  switch (rays_per_pixel) {
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
 * @brief Set value for tonemapping (to disable set to -1)
 */
void Scene::set_tonemapping_value(float tonemapping_gray) {
  _tonemapping_gray = tonemapping_gray;
}

/**
 * @brief Get pointer to the camera in the scene.
 *
 * @return Camera*
 */
Camera *Scene::get_camera(void) { return &_camera; }

/**
 * @brief Check if a Ray intersects with an Object in the Scene.
 *
 * @param ray
 * @param t_max maximum length of ray to check.
 * @return true if intersection found
 * @return false else
 */
bool Scene::check_intersection(Ray ray, float t_max) {
  for (size_t i = 0; i < _obj_spheres.size(); i++) {
    if ((_obj_spheres.data() + i)->intersect_bool(ray, t_max)) {
      return true;
    }
  }
  for (size_t i = 0; i < _obj_planes.size(); i++) {
    if ((_obj_planes.data() + i)->intersect_bool(ray, t_max)) {
      return true;
    }
  }
  for (size_t i = 0; i < _obj_meshes.size(); i++) {
    if ((_obj_meshes.data() + i)->intersect_bool(ray, t_max)) {
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
  // initialize_objects();
  Image image = Image(_camera.get_resolution().x, _camera.get_resolution().y);

  int resolution[2] = {static_cast<int>(_camera.get_resolution().x),
                       static_cast<int>(_camera.get_resolution().y)};

  uint count_pix = 0;

  // start time
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();
  std::cout << "------------------------------------------------\n";
  std::cout << "rendering\n\n";

  // #pragma omp parallel for num_threads(5)
  for (int x = 0; x < resolution[0]; x++) {
    std::cout << "\e[2K\e[1A"
              << "Progress: "
              << floorf(static_cast<float>(count_pix) /
                        (resolution[0] * resolution[1]) * 100)
              << "%\n";

    for (int y = 0; y < resolution[1]; y++) {
#ifdef DEBUG
      std::cout << "------------- new ray --------------------\n";
#endif
      // print progress

      // get color from ray
      vec3 color = vec3(0, 0, 0);
      for (size_t i = 0; i < _aliasing_positions.size(); i++) {
        vec3 light =
            get_light(_camera.get_ray({x, y}, _aliasing_positions.at(i), 0.2));

        if (light.x == -1) {
          light = _standart_light;
        }
        light *= 1.f / _aliasing_positions.size();
        color += light;
      }
      image.set_pixel({x, y}, color);
      count_pix++;
    }
  }
  if (_tonemapping_gray > 0) {
    image.apply_tonemapping(_tonemapping_gray);
  }
  // stop and print time
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "Time for rendering (sec) = "
            << (std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                      begin)
                    .count()) /
                   1000000.0
            << "\n";
  std::cout << "------------------------------------------------\n";
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
vec3 Scene::get_phong(const Pointlight &light, const Material &material,
                      vec3 point, vec3 normal, vec3 viewing_direction) {
  vec3 incoming_light = light.get_color();

  Ray ray_to_light = Ray(point, light.get_light_direction(point));
  ray_to_light.move_into_dir(0.01);

  vec3 light_direction = ray_to_light.get_direction();

  // normal * light_direction
  float ndotl = glm::dot(normal, light_direction);

  // angle less than 90 degrees no light reflected into direction
  if (ndotl < 0) {
    normal *= -1;
    ndotl = 0;
  }

  vec3 r = 2 * ndotl * normal - light_direction;

  float rdotv = glm::dot(r, viewing_direction);

  vec3 l_material = vec3(0, 0, 0);

  vec3 l_ambient = material.color * material.ambient;
  vec3 l_diffuse = material.diffuse * material.color;

  vec3 l_specular = vec3(0, 0, 0);
  if (rdotv > 0) {
    l_specular =
        material.specular * (vec3(1, 1, 1) * glm::pow(rdotv, material.pow_m));
  } else {
    rdotv *= -1;
    l_specular =
        material.specular * (vec3(1, 1, 1) * glm::pow(rdotv, material.pow_m));
  }
  // only ad the ones which are not blocked
  if (!check_intersection(ray_to_light, light.get_distance(point))) {
    l_material = l_diffuse + l_specular;
  }

  return incoming_light * ndotl * l_material + incoming_light * l_ambient;
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
vec3 Scene::get_mirroring_light(Material material, vec3 point, vec3 normal,
                                vec3 viewing_direction) {
  // perfect mirror
  vec3 color = vec3(0, 0, 0);
  if (material.mirror > 0) {
    color =
        get_light(generate_reflection_ray(point, normal, viewing_direction));
    if (color.x == -1) {
      color = vec3(0, 0, 0);
    }
  }
  return color;
}

void Scene::tonemapping(vec3 *light) {
  for (int i = 0; i < 3; i++) {
    (*light)[i] = (*light)[i] / (1 + (*light)[i]);
  }
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
vec3 Scene::calculate_light(const vec3 &point, const Material &material,
                            vec3 surface_normal, const Ray &ray) {
  vec3 res_light = vec3(0, 0, 0);

  vec3 v = ray.get_direction();

  surface_normal = glm::normalize(surface_normal);
  // calculate light for all lightsources
  vec3 mirror_light = get_mirroring_light(material, point, surface_normal, v);
  for (Pointlight light : _lights) {
    vec3 phong_light = get_phong(light, material, point, surface_normal, v);

    res_light += phong_light;
  }

  res_light =
      (res_light * (1 - material.mirror)) + (mirror_light * material.mirror);

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
Ray Scene::generate_reflection_ray(vec3 point, vec3 normal,
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
  for (size_t i = 0; i < _obj_spheres.size(); i++) {
    (_obj_spheres.data() + i)->update_view_transform(view_transform);
  }
  for (size_t i = 0; i < _obj_planes.size(); i++) {
    (_obj_planes.data() + i)->update_view_transform(view_transform);
  }
  for (size_t i = 0; i < _obj_meshes.size(); i++) {
    (_obj_meshes.data() + i)->update_view_transform(view_transform);
  }
  for (Pointlight light : _lights) {
    light.update_view_transform(view_transform);
  }
}

/**
 * @brief Calculates light transport in the scene along a ray.
 *
 * @param ray
 * @return amount of light reflected into ray directions.
 */
vec3 Scene::get_light(const Ray &ray) {
  // calculate object intersections
  Material material;
  Intersection best_intersection = {false, MAXFLOAT, vec3(0, 0, 0),
                                    vec3(0, 0, 0), material};

  // find closest intersection in Scene
  for (size_t i = 0; i < _obj_spheres.size(); i++) {
    Intersection intersect = (_obj_spheres.data() + i)->intersect(ray);

    if (intersect.found) {
      if (intersect.t <= best_intersection.t) {
        best_intersection = intersect;
      }
    }
  }
  for (size_t i = 0; i < _obj_planes.size(); i++) {
    Intersection intersect = (_obj_planes.data() + i)->intersect(ray);

    if (intersect.found) {
      if (intersect.t <= best_intersection.t) {
        best_intersection = intersect;
      }
    }
  }
  for (size_t i = 0; i < _obj_meshes.size(); i++) {
    Intersection intersect = (_obj_meshes.data() + i)->intersect(ray);

    if (intersect.found) {
      if (intersect.t <= best_intersection.t) {
        best_intersection = intersect;
      }
    }
  }
  if (best_intersection.found) {
    vec3 light =
        calculate_light(best_intersection.point, best_intersection.material,
                        best_intersection.normal, ray);

#ifdef NO_SHADING
    return best_intersection.material.color * vec3(255);
#endif
    return light;
  }
  // didn't hit any object
  return vec3(-1);
}
