/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "scene.hpp"
#include "objects/plane.hpp"

#include <glm/gtx/string_cast.hpp>
#include <memory>
#include <time.h>

// #define DEBUG

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
size_t Scene::add_light(Pointlight light) {
  _lights.push_back(light);
  return _lights.size() -1;
}

/**
 * @brief Add a Object to scenn.
 * 
 * @param object
 * @return size_t id of Object.
 */
size_t Scene::add_object(Object *object) {
  //_objects.push_back(object);
  return _objects.size() -1;
}

size_t Scene::add_object(Plane plane) {
  _obj_planes.push_back(plane);
  _objects.push_back(std::make_shared<Plane>(plane));
  //_objects.push_back(std::make_unique<Object>(new Plane(plane));
  //_objects.push_back(&_obj_planes.at(_obj_planes.size() - 1));
  return _objects.size() -1;
}
size_t Scene::add_object(Sphere sphere) {
  _obj_spheres.push_back(sphere);
  _objects.push_back(std::make_shared<Sphere>(sphere));
  //_objects.push_back(&_obj_spheres.at(_obj_spheres.size() - 1));
  return _objects.size() -1;
}
size_t Scene::add_object(Mesh mesh) {
  _obj_meshes.push_back(mesh);
  _objects.push_back(std::make_shared<Mesh>(mesh));
  //_objects.push_back(&_obj_meshes.at(_obj_meshes.size() - 1));
  return _objects.size() -1;
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
Camera *Scene::get_camera(void) {
  return &_camera;
}

/**
 * @brief Get pointer to object in the scene.
 * 
 * @param id id of the Object
 * @return Object* 
 */
//Object *Scene::get_object(size_t id) {
//  return _objects.at(id);
//}

/**
 * @brief Check if a Ray intersects with an Object in the Scene.
 * 
 * @param ray
 * @param t_max maximum length of ray to check.
 * @return true if intersection found
 * @return false else
 */
bool Scene::check_intersection(Ray ray, float t_max) {
  for (auto object : _objects) {
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



  uint count_pix = 0;

  // start time
  timeval start, end;
  gettimeofday(&start, 0);
  std::cout << "rendering\n\n";
  #pragma omp parallel for num_threads(5)
  for (int x = 0; x < resolution[0]; x++) {
      std::cout << "\e[2K\e[1A" << "Progress: "
      << floorf((float)count_pix / (resolution[0] * resolution[1]) * 100) << "%\n";

    for (int y = 0; y < resolution[1]; y++) {
      #ifdef DEBUG
      std::cout << "------------- new ray --------------------\n";
      #endif
      // print progress

      // get color from ray
      vec3 color = vec3(0, 0, 0);
      for (int i = 0; i < _aliasing_positions.size(); i++) {
        vec3 light =
          get_light(_camera.get_ray({x, y}, _aliasing_positions.at(i), 0.2));

        if (light.x == -1) {
          light = _standart_light;
        }
        light *= 1.f /_aliasing_positions.size();
        color += light;
      }
      image.set_pixel({x, y}, color);
      count_pix++;
    }
  }
  if (_tonemapping_gray > 0) {
    image.apply_tonemapping(_tonemapping_gray);
  }
  gettimeofday(&end, 0);
  std::cout << "Time for rendering: ";
  std::cout << end.tv_sec - start.tv_sec <<  " s\n";
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
    const Pointlight &light,
    const Material &material,
    vec3 point,
    vec3 normal,
    vec3 viewing_direction) {
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
    l_specular = material.specular *
        (vec3(1, 1, 1) * glm::pow(rdotv, material.pow_m));
  } else {
    rdotv *= -1;
    l_specular = material.specular * (vec3(1, 1, 1) *
        glm::pow(rdotv, material.pow_m));
  }
  // only ad the ones which are not blocked
  if (!check_intersection(ray_to_light, light.get_distance(point))) {
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
vec3 Scene::calculate_light(
    const vec3 &point,
    const Material &material,
    vec3 surface_normal,
    const Ray &ray) {

  vec3 res_light = vec3(0, 0, 0);

  vec3 v = ray.get_direction();

  surface_normal = glm::normalize(surface_normal);
  // calculate light for all lightsources
  vec3 mirror_light = get_mirroring_light(
      material, point, surface_normal, v);
  for (Pointlight light : _lights) {
    vec3 phong_light = get_phong(
        light, material, point, surface_normal, v);

    res_light += phong_light;
  }

  res_light = (res_light * (1 - material.mirror)) +
      (mirror_light * material.mirror);

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
  for (auto object : _objects) {
    object->update_view_transform(view_transform);
  }
  for (Pointlight light : _lights) {
    light.update_view_transform(view_transform);
  }
}

/// @brief Calculates light transport a long a ray in the scene.
/// @param ray
/// @return amount of light reflected to direction of ray.
vec3 Scene::get_light(const Ray& ray) {
  // calculate object intersections
  Material material;
  Intersection best_intersection = {
      false,
      MAXFLOAT,
      vec3(0, 0, 0),
      vec3(0, 0, 0),
      material};
  // find closest intersection in Scene
  for (auto object : _objects) {
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
