/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "mesh.h"

Mesh::Mesh(std::string input_file) {
  _triangle_exists = false;
  read_from_obj(input_file);
}

Mesh::~Mesh() {
  if (_triangle_exists) {
    delete[] _triangles;
    _triangle_exists = false;
  }
}

void Mesh::delete_triangles(void) {
}

void Mesh::print_triangles(void) {
  for (int i = 0; i < _size; i++) {
    _triangles[i].print();
  }
 }

int Mesh::get_size(void) { return _size; };

Triangle Mesh::get_triangle(int i) {
  Triangle t = _triangles[i];
  return t;
}

void Mesh::read_from_obj(std::string inputfile) {
tinyobj::attrib_t attrib;
std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;
  
std::string err;
std::string warn;

bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());
  
if (!err.empty()) { // `err` may contain warning message.
  std::cerr << err << std::endl;
}

if (!ret) {
  exit(1);
}


// shapes.size() number of objects
// shapes[s].mesh.num_face_vertices.size() -> number of triangle

if (shapes.size() > 1) {
  throw std::runtime_error("only support one mesh per obj at the moment");
}
// Loop over shapes
for (size_t s = 0; s < shapes.size(); s++) {
  // Loop over faces(polygon)
  // check if shape is triangle
  _size = shapes[s].mesh.num_face_vertices.size();
  _triangles = new Triangle[_size];
  _triangle_exists = true;

  size_t index_offset = 0;
  for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
    int fv = shapes[s].mesh.num_face_vertices[f];

    // Loop over vertices of one Triangle
    vec3 triangle_points[3];
    for (size_t v = 0; v < fv; v++) {
      // access to vertex
      tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
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
      triangle_points[v] = vec3(vx, vy, vz);
    }
    // make triangle and add to _triangles
    _triangles[f] = Triangle(triangle_points, vec3(200,0,0));

    index_offset += fv;

    // per-face material
    shapes[s].mesh.material_ids[f];
  }
}


}