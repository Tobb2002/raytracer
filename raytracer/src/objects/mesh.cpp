/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "mesh.hpp"
#include "lib/objloader.hpp"

#include <glm/gtx/string_cast.hpp>


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

void Mesh::move(vec3 vec) {
  for (size_t i = 0; i < _triangles.size(); i++) {
    Triangle *t = &_triangles[i];
    t->move(vec);
  }
  _bounding_box.move(vec);
}

/**
 * @brief Transform mesh (all triangles).
 * 
 * @param transformation transformation matrix.
 */
void Mesh::transform(mat4 transformation) {
  Object::transform(transformation);

  _bounding_box.transform(transformation);

  for (size_t i = 0; i < _triangles.size(); i++) {
    Triangle *t = &_triangles[i];
    t->transform(transformation);
    // check if t is outside of bounding box
    update_bounding_box(t);
  }
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

Intersection Mesh::intersect(Ray ray) {
  // calculate the intersection with the smallest value for t
  Triangle triangle_best;
  float t_min = MAXFLOAT;
  bool found_one = false;

  if (_bounding_box.intersect_bool(ray)) {
    for (Triangle triangle : _triangles) {
      Intersection t_i = triangle.intersect(ray);

      if (t_i.found && t_i.t < t_min) {
        found_one = true;
        t_min = t_i.t;
        triangle_best = triangle;
      }
    }
  }

  Intersection intersection = {found_one,
                               t_min,
                               ray.get_point(t_min),
                               triangle_best.get_normal(),
                               triangle_best.get_material()};

  return intersection;
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

  if (shapes.size() != 1) {
    throw std::runtime_error("only support one mesh per obj at the moment");
  }

  _size = shapes[0].mesh.num_face_vertices.size();


  size_t index_offset = 0;
  for (int f = 0; f < _size; f++) {
    int fv = shapes[0].mesh.num_face_vertices[f];

    // Loop over vertices of one Triangle
    vec3 triangle_points[3];
    for (int v = 0; v < fv; v++) {
      // access to vertex
      tinyobj::index_t idx = shapes[0].mesh.indices[index_offset + v];
      tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
      tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
      tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
      /* hier passiert Speicher zugriffsfehler
      tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
      tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
      tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
      tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
      tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
      */
      // Optional: vertex colors
      // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
      // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
      // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];

      // insert point into triangle_points
      triangle_points[v] = vec3(vx +_origin.x, vy + _origin.y, vz + _origin.z);
    }
    // make triangle and add to triangles
    _triangles.push_back(Triangle(triangle_points, _material));

    // update bounding box values
    update_bounding_box(&_triangles.back());

    index_offset += fv;
    // per-face material
    // shapes[s].mesh.material_ids[f];
  }
  _origin = _bounding_box.get_middle();
  _mat_translation = glm::translate(_mat_translation, _origin);

  calculate_inverse_mat();
}
