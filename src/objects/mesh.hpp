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

#define VISUALIZE_BVH false
#define LOAD_TEXTURES true

struct mesh_stats {
  uint intersects = 0;
  uint max_node_intersects = 0;
  uint min_node_intersects = UINT_MAX;
  uint node_intersects = 0;
  uint max_triangle_intersects = 0;
  uint min_triangle_intersects = UINT_MAX;
  uint triangle_intersects = 0;
  float time_building = 0;
};

class Mesh : public Object {
 public:
  Mesh(std::string folder, std::string file, vec3 origin);
  Mesh(std::string folder, std::string file, vec3 origin, Material material,
       Algorithm algorithm = ASAH);
  Mesh(std::string folder, std::string file, vec3 origin, Material material,
       std::string texture_path, Algorithm algorithm = ASAH);

  Mesh(const Mesh& old_mesh);
  Mesh& operator=(const Mesh& old_mesh);

  /***** Print Debug information *****/
  void print_triangles(void);
  void print_bounding_box(void);
  void print(void) override;
  mesh_stats get_stats(void);

  /***** Transformations *****/

  void apply_transform(mat4 transformation) override;

  /***** getters *****/
  int get_size(void);
  Triangle get_triangle(int i);

  /***** Functions *****/
  Intersection intersect(const Ray& ray) override;
  Intersection get_intersect(const TriangleIntersection triangle_intersect);

  void print_stats();
  void print_triangle_stats();

 private:
  std::vector<Triangle> _triangles;
  bool _triangle_exists;
  int _size;
  vec3 _origin;
  Box _bounding_box;
  BVH _bvh;
  UniformGrid _grid;

  bool _enable_smooth_shading = true;

  Material _material_default;
  std::vector<Material> _materials;
  std::vector<Texture> _textures_diffuse;
  std::vector<Texture> _textures_specular;
  std::vector<Texture> _textures_normal;

  std::string _path_folder;

  Texture _texture;
  bool _enable_texture = false;

  mesh_stats _stats;

  /// @brief updates the intersection stats
  void update_stats(bvh_stats bvh_stats);

  void build_datastructure();

  // define data structure to use
  Algorithm _used_algorithm = ASAH;

  void update_bounding_box(Triangle* t);
  void read_from_obj(std::string folder, std::string file);
};
