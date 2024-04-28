/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "mesh.hpp"
#include "lib/objloader.hpp"
#include "bvh.hpp"

#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <chrono>


/**
 * @brief Construct a new Mesh:: Mesh object
 * 
 * @param input_file ppm file to generate mesh.
 * @param origin point to place the mesh.
 * 
 * The Mesh will use the standart Material.
 */
Mesh::Mesh(std::string input_file, vec3 origin) {
  _origin = origin;
  read_from_obj(input_file);  // read file with origin as offset
}

/**
 * @brief Construct a new Mesh:: Mesh object
 * 
 * @param input_file ppm file to generate mesh.
 * @param origin point to place the mesh.
 * @param material set material of mesh.
 */
Mesh::Mesh(std::string input_file, vec3 origin, Material material) {
  _origin = origin;
  _material = material;
  read_from_obj(input_file);  // read file with origin as offset

  // stop time needed to build bvh
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();
  _bvh.build_tree(&_triangles);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "Time for building bvh (sec) = " <<
    (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
    / 1000000.0 << "\n";
}

Mesh::Mesh(const Mesh &old_mesh) {
  _triangles = old_mesh._triangles;
  _size = old_mesh._size;
  _origin = old_mesh._origin;
  _bounding_box = old_mesh._bounding_box;
  _enable_smooth_shading = old_mesh._enable_smooth_shading;
  _material = old_mesh._material;
  _bvh = old_mesh._bvh;

  // set new triangle reference
  _bvh.set_triangles(&_triangles);
}

Mesh::~Mesh() {}

/***** Print DEBUG information *****/

/**
 * @brief Print information about mesh.
 */
void Mesh::print(void) {
  Object::print();
  std::cout << "------mesh------\n";
  std::cout << "origin: " << glm::to_string(_origin) << "\n";
  print_bounding_box();
}

/**
 * @brief Print all triangles of the mesh.
 * 
 */
void Mesh::print_triangles(void) {
  for (int i = 0; i < _size; i++) {
    std::cout << "id: " << i << "\n";
    _triangles[i].print();
  }
}

/**
 * @brief Print information about Boundingbox.
 */
void Mesh::print_bounding_box(void) {
  _bounding_box.print();
}

/***** Getters *****/

/**
 * @brief Get number of triangles in mesh.
 * 
 * @return int 
 */
int Mesh::get_size(void) { return _size; }

/**
 * @brief Get a specific Triangle from the mesh.
 * 
 * @param i id of that triangle.
 * @return Triangle 
 */
Triangle Mesh::get_triangle(int i) {
  Triangle t = _triangles.at(i);
  return t;
}

/***** Transformations *****/

/**
 * @brief Transform mesh (all triangles).
 * 
 * @param transformation transformation matrix.
 */
void Mesh::apply_transform(mat4 transformation) {
  Object::apply_transform(transformation);

  _bounding_box.apply_transform(transformation);

  for (size_t i = 0; i < _triangles.size(); i++) {
    Triangle *t = &_triangles[i];
    t->apply_transform(transformation);
  }
  _bvh.update_boxes();
}

/**
 * @brief Check if Triangle is in bounding box and update if not.
 * 
 * @param t Triangle to check.
 */
void Mesh::update_bounding_box(Triangle * t) {
  _bounding_box.update_min_max(t->get_min_bounding(),
                               t->get_max_bounding());
}

/***** Functions *****/

Intersection Mesh::intersect(const Ray& ray) {
  return _bvh.intersect(ray);
}

/***** File input *****/

/**
 * @brief read triangles from objfile.
 * 
 * @param inputfile path to obj file.
 */
void Mesh::read_from_obj(std::string inputfile) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err;
  std::string warn;

  bool ret = tinyobj::LoadObj(&attrib, &shapes,
                              &materials, &warn,
                              &err, inputfile.c_str());

  if (!err.empty()) {  // `err` may contain warning message.
    std::cerr << err << std::endl;
  }
  if (!ret) {
    exit(1);
  }

  // shapes.size() number of objects
  // shapes[s].mesh.num_face_vertices.size() -> number of triangle

  std::cout << "shapes:" << shapes.size() << "\n";

  // check if texture coordinates available
  bool texture_available = false;
  if (attrib.texcoords.size() > 0) {
    texture_available = true;
  }

  // check if vertex normals are available
  bool vertex_normals_available = false;
  if (attrib.normals.size() > 0) {
    vertex_normals_available = true;
    std::cout << "normals available\n";
  }

  for (int s = 0; s < shapes.size(); s++) {
    int size = shapes[s].mesh.num_face_vertices.size();
    std::cout << "size:" << size << "\n";


    size_t index_offset = 0;
    for (int f = 0; f < size; f++) {
      int fv = shapes[s].mesh.num_face_vertices[f];

      // Loop over vertices of one Triangle
      vec3 triangle_points[3];
      vec2 triangle_points_uv[3];
      vec3 triangle_normals[3];
      for (int v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[0].mesh.indices[index_offset + v];
        tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
        tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
        tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];

        // texture coordinates
        tinyobj::real_t tx = -1;
        tinyobj::real_t ty = -1;

        if (texture_available) {
          tx = attrib.texcoords[2*idx.texcoord_index+0];
          ty = attrib.texcoords[2*idx.texcoord_index+1];
        }

        // vertex normals
        tinyobj::real_t nx = 0;
        tinyobj::real_t ny = 0;
        tinyobj::real_t nz = 0;

        if (vertex_normals_available) {
          nx = attrib.normals[3*idx.normal_index+0];
          ny = attrib.normals[3*idx.normal_index+1];
          nz = attrib.normals[3*idx.normal_index+2];
        }
        // Optional: vertex colors
        // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
        // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
        // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];

        // insert point into triangle_points
        triangle_points[v] = vec3(vx +_origin.x,
                                  vy + _origin.y,
                                  vz + _origin.z);
        triangle_points_uv[v] = vec2(tx, ty);
        triangle_normals[v] = vec3(nx, ny, nz);
      }
      // make triangle and add to triangles
      Triangle t = Triangle(triangle_points, _material);
      if (vertex_normals_available && _enable_smooth_shading) {
        t.set_vertex_normals(triangle_normals);
      }
      _triangles.push_back(t);

      // update bounding box values
      update_bounding_box(&_triangles.back());

      index_offset += fv;
      // per-face material
      // shapes[s].mesh.material_ids[f];
    }
  }
  _origin = _bounding_box.get_middle();
  _transform.add_translation(_origin);
}
