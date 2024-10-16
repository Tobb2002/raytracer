/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "mesh.hpp"

#include <algorithm>
#include <chrono>
#include <execution>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <mutex>

#include "bvh.hpp"
#include "lib/objloader.hpp"

/**
 * @brief Construct a new Mesh:: Mesh object
 *
 * @param input_file ppm file to generate mesh.
 * @param origin point to place the mesh.
 *
 * The Mesh will use the standart Material.
 */
Mesh::Mesh(std::string folder, std::string file, vec3 origin) {
  _origin = origin;
  _path_folder = folder;
  read_from_obj(folder, file);  // read file with origin as offset
}

/**
 * @brief Construct a new Mesh:: Mesh object
 *
 * @param input_file ppm file to generate mesh.
 * @param origin point to place the mesh.
 * @param material set material of mesh.
 */
Mesh::Mesh(std::string folder, std::string file, vec3 origin, Material material,
           Algorithm algorithm) {
  _origin = origin;
  _path_folder = folder;
  _material_default = material;
  _materials.push_back(material);
  read_from_obj(folder, file);  // read file with origin as offset
  _used_algorithm = algorithm;

  build_datastructure();
}

Mesh::Mesh(std::string folder, std::string file, vec3 origin, Material material,
           std::string texture_path, Algorithm algorithm) {
  _origin = origin;
  _path_folder = folder;
  _material_default = material;
  _materials.push_back(material);
  read_from_obj(folder, file);  // read file with origin as offset
  _used_algorithm = algorithm;

  // load and enable texture
  _enable_texture = true;
  _texture.load_image(texture_path);

  build_datastructure();
}

void Mesh::build_datastructure() {
  // stop time needed to build bvh
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();
  switch (_used_algorithm) {
    case AGRID:
      std::cout << "Algorithm: Uniform Grid\n";
      _grid.build(&_triangles);
      break;
    default:
      _bvh.build_tree_axis(&_triangles, _used_algorithm);
      break;
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  _stats.time_building =
      (std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
           .count()) /
      1000000.0;
  std::cout << "------------------------------------------------\n";
  std::cout << "Time for building bvh (sec) = ";
  std::cout << _stats.time_building << "\n";
  std::cout << "------------------------------------------------\n";
}

Mesh::Mesh(const Mesh &old_mesh) {
  _triangles = old_mesh._triangles;
  _triangle_exists = old_mesh._triangle_exists;
  _size = old_mesh._size;
  _origin = old_mesh._origin;
  _bounding_box = old_mesh._bounding_box;
  _enable_smooth_shading = old_mesh._enable_smooth_shading;
  _materials = old_mesh._materials;
  _bvh = old_mesh._bvh;
  _texture = old_mesh._texture;
  _enable_texture = old_mesh._enable_texture;
  _grid = old_mesh._grid;
  _used_algorithm = old_mesh._used_algorithm;
  _stats = old_mesh._stats;
  _textures_diffuse = old_mesh._textures_diffuse;
  _textures_specular = old_mesh._textures_specular;

  // set new triangle reference
  _bvh.set_triangles(&_triangles);
  _grid.set_triangles(&_triangles);
}

Mesh &Mesh::operator=(const Mesh &old_mesh) {
  _triangles = old_mesh._triangles;
  _triangle_exists = old_mesh._triangle_exists;
  _size = old_mesh._size;
  _origin = old_mesh._origin;
  _bounding_box = old_mesh._bounding_box;
  _enable_smooth_shading = old_mesh._enable_smooth_shading;
  _materials = old_mesh._materials;
  _bvh = old_mesh._bvh;
  _texture = old_mesh._texture;
  _enable_texture = old_mesh._enable_texture;
  _grid = old_mesh._grid;
  _used_algorithm = old_mesh._used_algorithm;
  _stats = old_mesh._stats;
  _textures_diffuse = old_mesh._textures_diffuse;
  _textures_specular = old_mesh._textures_specular;

  // set new triangle reference
  _bvh.set_triangles(&_triangles);
  _grid.set_triangles(&_triangles);

  return *this;
}

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

mesh_stats Mesh::get_stats(void) { return _stats; }

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
void Mesh::print_bounding_box(void) { _bounding_box.print(); }

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
  build_datastructure();
}

/**
 * @brief Check if Triangle is in bounding box and update if not.
 *
 * @param t Triangle to check.
 */
void Mesh::update_bounding_box(Triangle *t) {
  _bounding_box.update_min_max(t->get_min_bounding(), t->get_max_bounding());
}

/***** Functions *****/

Intersection Mesh::intersect(const Ray &ray) {
  TriangleIntersection intersect_triangle;
  switch (_used_algorithm) {
    case AGRID:
      intersect_triangle = _grid.intersect(ray);
      break;
    default:
      intersect_triangle = _bvh.intersect(ray);
#if GET_STATS
      update_stats(_bvh.get_stats());
#endif
      break;
  }

  return get_intersect(intersect_triangle);
}

Intersection Mesh::get_intersect(const TriangleIntersection t_intersect) {
  Intersection res = {t_intersect.found, t_intersect.t, t_intersect.point,
                      t_intersect.normal,
                      _materials.at(t_intersect.material_id)};

  if (_enable_texture) {
    if (res.material.texture_id_diffuse >= 0) {
      res.material.color = _textures_diffuse.at(res.material.texture_id_diffuse)
                               .get_color_uv(t_intersect.texture_uv);
    }
    if (res.material.texture_id_specular >= 0) {
      vec3 spec = _textures_specular.at(res.material.texture_id_specular)
                      .get_color_uv(t_intersect.texture_uv);
      res.material.specular = spec;

      // if (spec.z >= 0.5) {
      // res.material.mirror =
      //     _textures_specular.at(res.material.texture_id_specular)
      //         .get_color_uv(t_intersect.texture_uv).z;
      // }
    }
  }
  // calculate normals if
  // res.normal =
  // _textures_normal.at(res.material.texture_id_normal).get_normal_uv(t_intersect.normal_uv);
#if VISUALIZE_BVH
  res.material.color = _bvh.get_stats().intersection_color;
#endif
  return res;
}

/***** File input *****/

/**
 * @brief read triangles from objfile.
 *
 * @param inputfile path to obj file.
 */
void Mesh::read_from_obj(std::string folder, std::string file) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err;
  std::string warn;

  std::string inputfile = folder + "/" + file;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                              inputfile.c_str(), folder.c_str());

  if (!err.empty()) {  // `err` may contain warning message.
    std::cerr << err << std::endl;
  }
  if (!ret) {
    exit(1);
  }

  // shapes.size() number of objects
  // shapes[s].mesh.num_face_vertices.size() -> number of triangle

  std::cout << "------------------------------------------------\n";
  std::cout << "obj file:\n";

  std::cout << "number of objects:" << shapes.size() << "  ";

  std::cout << "vertex data: ";

  // check if texture coordinates available
  bool texture_available = false;
  if (attrib.texcoords.size() > 0) {
    texture_available = true;
    std::cout << "texture ";
  }

  // check if vertex normals are available
  bool vertex_normals_available = false;
  if (attrib.normals.size() > 0) {
    vertex_normals_available = true;
    std::cout << "normals";
  }

  // read materials
  if (materials.size() > 1) {
    _materials.clear();
    std::for_each(
        std::execution::seq, materials.begin(), materials.end(),
        [this](tinyobj::material_t material) {
          Texture t_diffuse = Texture();

#if LOAD_TEXTURES
          if (material.diffuse_texname.length() > 0) {
            Texture t_diffuse =
                Texture(_path_folder + "/" + material.diffuse_texname);
            _enable_texture = true;
            _textures_diffuse.push_back(t_diffuse);
          }
          if (material.specular_texname.length() > 0) {
            Texture t_specular =
                Texture(_path_folder + "/" + material.specular_texname);
            _textures_specular.push_back(t_specular);
          }
#endif
          int texture_id_diffuse = _textures_diffuse.size() - 1;
          int texture_id_specular = _textures_specular.size() - 1;
          _materials.push_back(
              {.color = vec3(material.diffuse[0], material.diffuse[1],
                             material.diffuse[2]),
               .ambient = _material_default.ambient,
               .specular = vec3(material.specular[0], material.specular[1],
                                material.specular[2]),
               .pow_m = static_cast<float>(material.shininess),
               .mirror = _material_default.mirror,
               .texture_id_diffuse = texture_id_diffuse,
               .texture_id_specular = texture_id_specular});
        });
  }

  for (size_t s = 0; s < shapes.size(); s++) {
    int size = shapes[s].mesh.num_face_vertices.size();
    std::cout << "object(" << s << ") size:" << size << "\n";

    size_t index_offset = 0;
    for (int f = 0; f < size; f++) {
      int fv = shapes[s].mesh.num_face_vertices[f];

      // Loop over vertices of one Triangle
      vec3 triangle_points[3];
      vec2 triangle_points_uv[3];
      vec3 triangle_normals[3];
      for (int v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
        tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
        tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

        // texture coordinates
        tinyobj::real_t tx = -1;
        tinyobj::real_t ty = -1;

        if (texture_available) {
          tx = attrib.texcoords[2 * idx.texcoord_index + 0];
          ty = attrib.texcoords[2 * idx.texcoord_index + 1];
        }

        // vertex normals
        tinyobj::real_t nx = 0;
        tinyobj::real_t ny = 0;
        tinyobj::real_t nz = 0;

        if (vertex_normals_available) {
          nx = attrib.normals[3 * idx.normal_index + 0];
          ny = attrib.normals[3 * idx.normal_index + 1];
          nz = attrib.normals[3 * idx.normal_index + 2];
        }
        // Optional: vertex colors
        // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
        // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
        // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];

        // insert point into triangle_points
        triangle_points[v] =
            vec3(vx + _origin.x, vy + _origin.y, vz + _origin.z);
        triangle_points_uv[v] = vec2(tx, ty);
        triangle_normals[v] = vec3(nx, ny, nz);
      }
      // add triangle to triangles vector
      _triangles.emplace_back(triangle_points, 0);

      // add properties of triangle
      Triangle *t = _triangles.data() + _triangles.size() - 1;
      if (vertex_normals_available && _enable_smooth_shading) {
        t->set_vertex_normals(triangle_normals);
      }
      if (texture_available) {
        t->set_vertex_texture(triangle_points_uv);
      }

      // per-face material
      if (materials.size() > 1) {
        t->set_material(shapes[s].mesh.material_ids[f]);
      }

      // update bounding box values
      update_bounding_box(&_triangles.back());

      index_offset += fv;
    }
  }
  std::cout << "materials used: " << _materials.size() << "\n";
  std::cout << "material 0: " << _materials.at(0).ambient.y << "\n";
  std::cout << "------------------------------------------------\n";
  std::cout << "root bounds surface area: " << _bounding_box.get_surface_area()
            << "\n";
  std::cout << "------------------------------------------------\n";

  _origin = _bounding_box.get_middle();
  _transform.add_translation(_origin);
}

void Mesh::update_stats(bvh_stats bvh_stats) {
  _stats.intersects += 1;
  _stats.node_intersects += bvh_stats.node_intersects;
  _stats.triangle_intersects += bvh_stats.triangle_intersects;
  _stats.intersection_time_all += bvh_stats.intersection_time;

  // update min max values
  if (bvh_stats.node_intersects < _stats.min_node_intersects) {
    _stats.min_node_intersects = bvh_stats.node_intersects;
  }
  if (bvh_stats.node_intersects > _stats.max_node_intersects) {
    _stats.max_node_intersects = bvh_stats.node_intersects;
  }
  if (bvh_stats.triangle_intersects < _stats.min_triangle_intersects) {
    _stats.min_triangle_intersects = bvh_stats.triangle_intersects;
  }
  if (bvh_stats.triangle_intersects > _stats.max_triangle_intersects) {
    _stats.max_triangle_intersects = bvh_stats.triangle_intersects;
  }
}

void Mesh::print_stats() {
  std::cout << "------------------------------------------------\n";
  std::cout << "Mesh stats: \n";
  std::cout << "BVH Nodes intersected: \n";
  std::cout << "\t all: \t" << _stats.node_intersects << "\n";
  std::cout << "\t min: \t" << _stats.min_node_intersects << "\n";
  std::cout << "\t max: \t" << _stats.max_node_intersects << "\n";
  if (_stats.intersects != 0) {
    std::cout << "\t avg: \t"
              << static_cast<float>(_stats.node_intersects) / _stats.intersects
              << "\n";
  }
  std::cout << "BVH triangles intersected: \n";
  std::cout << "\t all: \t" << _stats.triangle_intersects << "\n";
  std::cout << "\t min: \t" << _stats.min_triangle_intersects << "\n";
  std::cout << "\t max: \t" << _stats.max_triangle_intersects << "\n";
  if (_stats.intersects != 0) {
    std::cout << "\t avg: \t"
              << static_cast<float>(_stats.triangle_intersects) /
                     _stats.intersects
              << "\n";
  }
  std::cout << "Intersection time: \n";
  std::cout << "\t all: \t" << _stats.intersection_time_all << "\n";
  std::cout << "\t avg: \t" << _stats.intersection_time_all / _stats.intersects
            << "\n";
  std::cout << "------------------------------------------------\n";
}
void Mesh::print_triangle_stats() {
  vec3 combined_lenght = vec3(0);
  for (Triangle t : _triangles) {
    vec3 length = glm::abs(t.get_max_bounding() - t.get_min_bounding());

    combined_lenght += length;
  }
  std::cout << "------------------------------------------------\n";
  combined_lenght /= _triangles.size() - 1;
  std::cout << "avg_triangle size: " << combined_lenght.x << ", "
            << combined_lenght.y << ", " << combined_lenght.z << "\n";
  std::cout << "... surface area: "
            << 2 * combined_lenght.x + 2 * combined_lenght.y +
                   2 * combined_lenght.z
            << "\n";
  std::cout << "------------------------------------------------\n";
}
