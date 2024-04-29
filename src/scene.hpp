/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <vector>

#include "objects/mesh.hpp"
#include "objects/object.hpp"
#include "objects/plane.hpp"
#include "objects/pointlight.hpp"
#include "objects/camera.hpp"
#include "image.hpp"
#include "objects/sphere.hpp"
#include "memory"


class Scene {
 public:
  explicit Scene(vec3 standart_color);

  /***** Adding things to scene *****/

  size_t add_light(Pointlight light);
  size_t add_object(Plane plane);
  size_t add_object(Sphere sphere);
  size_t add_object(Mesh mesh);

  /***** Change Scene Settings *****/

  void set_aliasing(uint rays_per_pixel);
  void set_tonemapping_value(float tonemapping_gray);

  /***** Getters *****/

  Camera *get_camera(void);

  vec3 get_light(const Ray& ray);

  void update_view_transform(void);


  /***** Rendering *****/

  Image trace_image();

 private:
  std::vector<Pointlight> _lights;
  std::vector<std::shared_ptr<Object>> _objects;

  std::vector<Plane> _obj_planes;
  std::vector<Sphere> _obj_spheres;
  std::vector<Mesh> _obj_meshes;

  Camera _camera;

  /// @brief light if a ray hit's nothing
  vec3 _standart_light;
  float _tonemapping_gray = 0.8;
  std::vector<vec2> _aliasing_positions;

  Ray generate_reflection_ray(vec3 point, vec3 normal, vec3 viewer_direction);


  vec3 calculate_light(
      const vec3 &point,
      const Material &material,
      vec3 surface_normal,
      const Ray &camera_ray);
  vec3 get_phong(
      const Pointlight &light,
      const Material &material,
      vec3 point,
      vec3 normal,
      vec3 viewing_direction);
  vec3 get_mirroring_light(
      Material material,
      vec3 point,
      vec3 normal,
      vec3 viewing_direction);
  void tonemapping(vec3 *light);
  bool check_intersection(Ray ray, float t_max);
};
