/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "mesh.hpp"
#include "lib/objloader.hpp"


Mesh::Mesh(std::string input_file, vec3 origin) {
  _origin = origin;
  read_from_obj(input_file);
}

Mesh::~Mesh() {}

void Mesh::print_triangles(void) {
  for (int i = 0; i < _size; i++) {
    _triangles[i].print();
  }
}

int Mesh::get_size(void) { return _size; }

Triangle Mesh::get_triangle(int i) {
  Triangle t = _triangles.at(i);
  return t;
}

void Mesh::move(vec3 vec) {
  for (size_t i = 0; i < _triangles.size(); i++) {
    Triangle *t = &_triangles[i];
    t->move(vec);
  }
  _bounding_box.move(vec);
}

void Mesh::print(void) {
  Object::print();
  std::cout << "mesh\n";
  print_bounding_box();
}

void Mesh::print_bounding_box(void) {
  _bounding_box.print();
}

Intersection Mesh::intersect(Ray ray) {
  // calculate the intersection with the smallest value for t
  Triangle triangle_best;
  float t_min = MAXFLOAT;
  bool found_one = false;

  if (_bounding_box.intersect_bool(ray)) {
    for (Triangle triangle : _triangles) {
      float t = triangle.intersect(ray);

      if (t > 0 && t < t_min) {
        found_one = true;
        t_min = t;
        triangle_best = triangle;
      }
    }
  }

  Intersection intersection = {found_one,
                               t_min,
                               ray.get_point(t_min),
                               triangle_best.get_normal(),
                               triangle_best.get_color()};

  return intersection;
}


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

  // initialize values for bounding box
  vec3 box_min = vec3(FLT_MAX);
  vec3 box_max = vec3(-FLT_MAX);

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

      // update bounding box values
      // min values
      if (triangle_points[v].x < box_min.x) {
        box_min.x = triangle_points[v].x;
      }
      if (triangle_points[v].y < box_min.y) {
        box_min.y = triangle_points[v].y;
      }
      if (triangle_points[v].z < box_min.z) {
        box_min.z = triangle_points[v].z;
      }
      // max values
      if (triangle_points[v].x > box_max.x) {
        box_max.x = triangle_points[v].x;
      }
      if (triangle_points[v].y > box_max.y) {
        box_max.y = triangle_points[v].y;
      }
      if (triangle_points[v].z > box_max.z) {
        box_max.z = triangle_points[v].z;
      }
    }
    // make triangle and add to triangles
    _triangles.push_back(Triangle(triangle_points, vec3(1, 0, 0)));

    index_offset += fv;

    // per-face material
    // shapes[s].mesh.material_ids[f];
  }
  // set bounding box values
  _bounding_box.set_min_max(box_min, box_max);
}
