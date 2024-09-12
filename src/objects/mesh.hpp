/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#pragma once

#include <iostream>
#include <vector>

#include "box.hpp"
#include "bvh.hpp"
#include "object.hpp"
#include "texture.hpp"
#include "triangle.hpp"
#include "uniform_grid.hpp"

struct mesh_stats {
  long intersects = 0;
  uint max_node_intersects = 0;
  uint min_node_intersects = UINT_MAX;
  long node_intersects = 0;
  uint max_triangle_intersects = 0;
  uint min_triangle_intersects = UINT_MAX;
  long triangle_intersects = 0;
};

class Mesh : public Object {
 public:
  Mesh(std::string input_file, vec3 origin);
  Mesh(std::string input_file, vec3 origin, Material material,
       Algorithm algorithm = ASAH);
  Mesh(std::string input_file, vec3 origin, Material material,
       std::string texture_path, Algorithm algorithm = ASAH);

  Mesh(const Mesh& old_mesh);
  Mesh& operator=(const Mesh& old_mesh);

  /***** Print Debug information *****/
  void print_triangles(void);
  void print_bounding_box(void);
  void print(void) override;

  /***** Transformations *****/

  void apply_transform(mat4 transformation) override;

  /***** getters *****/
  int get_size(void);
  Triangle get_triangle(int i);

  /***** Functions *****/
  Intersection intersect(const Ray& ray) override;

  void print_stats();

 private:
  std::vector<Triangle> _triangles;
  bool _triangle_exists;
  int _size;
  vec3 _origin;
  Box _bounding_box;
  BVH _bvh;
  UniformGrid _grid;

  bool _enable_smooth_shading = true;

  Material _material;
  Texture _texture;
  bool _enable_texture = false;

  mesh_stats _stats;

  /// @brief updates the intersection stats
  void update_stats(bvh_stats bvh_stats);

  void build_datastructure();

  // define data structure to use
  Algorithm _used_algorithm = ASAH;

  void update_bounding_box(Triangle* t);
  void read_from_obj(std::string input_file);
};
