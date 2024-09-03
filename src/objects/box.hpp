/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "object.hpp"
#include "ray.hpp"
#include "triangle.hpp"
using glm::vec3;

struct bvh_box {
  vec3 min = vec3(0);
  vec3 max = vec3(0);
};

struct interval {
  float min;
  float max;
};

/// @brief intersect bounding box and return ray parameter t of intersection
/// point. return -1 if no intersection
float intersect_bounds(const bvh_box& box, const Ray& ray);

bvh_box calculate_bounds(std::vector<Triangle>* triangles);

/**
 * @brief Datastructure to represent bounding_boxes.
 *
 * Bounding box get's defined by a min and max value.
 */
class Box : public Object {
 public:
  Box();
  Box(vec3 min, vec3 max);

  void set_min_max(vec3 min, vec3 max);
  void update_min_max(vec3 min, vec3 max);
  bool intersect_bool(Ray ray);
  vec3 get_middle(void);

  // Transformations
  void apply_transform(mat4 transformation) override;

  void print(void) override;

 private:
  vec3 _min;
  vec3 _max;

  void ensure_min_max(void);
};
