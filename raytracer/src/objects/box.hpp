/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <glm/glm.hpp>
#include "ray.hpp"
#include "object.hpp"

using glm::vec3;

struct interval {
  float min;
  float max;
};

/**
 * @brief Datastructure to represent bounding_boxes.
 * 
 * Bounding box get's defined by a min and max value.
 */
class Box : public Object{
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
  vec3 _max;;


  void ensure_min_max(void);
};
