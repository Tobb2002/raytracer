/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include <cmath>
#include <vector>

#include "box.hpp"
#include "bvh_tree.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "bvh.hpp"
#include "lbvh.hpp"

// #define VISUALIZE_INTERSECT

// #define FLATTEN_TREE

#define USE_LBVH  // SPLIT_MID, SPLIT_SAH, USE_LBVH ...

void BVH::build_tree_axis(std::vector<Triangle> *triangles) {
  // initialize data structure
  _data.triangles = triangles;

  uint size = _data.triangles->size();
  BVH_node_data root_data = BVH_node_data();
  root_data.triangle_ids.reserve(size);
  for (size_t i = 0; i < size; i++) {
    root_data.triangle_ids.push_back(i);
  }
  _data.tree = BVH_tree(root_data, triangles);
  bvh_node_pointer *root = _data.tree.get_root();

  _data.tree.calculate_bounds(root);

#ifdef SPLIT_MID
  SAH sah = SAH(&_data.tree);
  sah.split_middle(root);
#endif

#ifdef SPLIT_SAH
  SAH sah = SAH(&_data.tree);
  sah.split(root);
#endif
#ifdef USE_LBVH
  LBVH l = LBVH(&_data.tree);
  // lbvh.sort();
  l.build();
#endif

#ifdef FLATTEN_TREE
  _data.tree.flatten_tree();
#endif
}

void BVH::set_triangles(std::vector<Triangle> *triangles) {
  _data.triangles = triangles;
  _data.tree.set_triangles(triangles);
}

Intersection BVH::intersect(const Ray &ray) {
  _intersect_count = 0;
#ifdef FLATTEN_TREE
  return intersect_node((uint)0, ray);
#else
  return intersect_node(_data.tree.get_root(), ray);
#endif
}

/**
 * @brief recursivley calculate intersection in nodes
 *
 * @param node_id
 * @param ray
 * @return Intersection
 */
Intersection BVH::intersect_node(bvh_node_pointer *node, const Ray &ray) {
  Intersection result;
  if (!intersect_node_bool(_data.tree.get_data(node), ray)) {
    return result;
  }

  _intersect_count += 1;
  // check if leaf
  if (_data.tree.is_leaf(node)) {
    Intersection intersect = intersect_leaf(_data.tree.get_data(node), ray);

    return intersect;
  }

  update_intersection(&result, intersect_node(_data.tree.get_left(node), ray));
  update_intersection(&result, intersect_node(_data.tree.get_right(node), ray));

  return result;
}

Intersection BVH::intersect_node(uint id_flat, const Ray &ray) {
  Intersection result;
  if (!intersect_node_bool((_data.tree.get_data(id_flat)), ray)) {
    return result;
  }

  _intersect_count += 1;
  // check if leaf
  if (_data.tree.get_node(id_flat)->is_leaf) {
    Intersection intersect = intersect_leaf(_data.tree.get_data(id_flat), ray);

    return intersect;
  }

  update_intersection(&result, intersect_node(id_flat + 1, ray));
  update_intersection(&result,
                      intersect_node(_data.tree.get_right(id_flat), ray));

  return result;
}
/**
 * @brief check if hitbox of node has intersection.
 *
 * @param id
 * @param ray
 * @return true
 * @return false
 */
bool BVH::intersect_node_bool(BVH_node_data *node_data, const Ray &ray) {
  vec3 d = ray.get_direction();
  vec3 o = ray.get_origin();

  vec3 box_min = node_data->bounds.min;
  vec3 box_max = node_data->bounds.max;

  Interval tx = {(box_min.x - o.x) / d.x, (box_max.x - o.x) / d.x};

  Interval ty = {(box_min.y - o.y) / d.y, (box_max.y - o.y) / d.y};

  Interval tz = {(box_min.z - o.z) / d.z, (box_max.z - o.z) / d.z};

  // overlapping intervals indicate intersection
  // check box intersection

  if (tx.min > tx.max) {
    float t = tx.min;
    tx.min = tx.max;
    tx.max = t;
  }
  if (ty.min > ty.max) {
    float t = ty.min;
    ty.min = ty.max;
    ty.max = t;
  }
  if (tz.min > tz.max) {
    float t = tz.min;
    tz.min = tz.max;
    tz.max = t;
  }

  if (tx.min > ty.max || ty.min > tx.max) {
    // xy do not ovelap
    return false;
  }
  if (tx.min > tz.max || tz.min > tx.max) {
    // xz do not overlap
    return false;
  }
  if (ty.min > tz.max || tz.min > ty.max) {
    // yz do not overlap
    return false;
  }

  // all intervalls overlap
  return true;
}

Intersection BVH::intersect_leaf(BVH_node_data *node_data, const Ray &ray) {
#ifdef DEBUG
  std::cout << "id: " << id << "  parent:" << (id - 1) / 2 << "\n";
#endif
  // find best intersection in triangle set
  uint best_triangle_id = 0;
  float t_min = MAXFLOAT;

  for (uint i : node_data->triangle_ids) {
    Intersection t_i = _data.triangles->at(i).intersect(ray);

    if (t_i.found && t_i.t < t_min) {
      t_min = t_i.t;
      best_triangle_id = i;
    }
  }

#ifndef VISUALIZE_INTERSECT
  return (_data.triangles->data() + best_triangle_id)->intersect(ray);
#else
  Intersection res =
      (_data.triangles->data() + best_triangle_id)->intersect(ray);
  if (_intersect_count < 255) {
    res.material.color.x = _intersect_count;
    res.material.color.y = 255 - _intersect_count;
  }
  _intersect_count = 0;
  return res;
#endif
}

bool BVH::update_intersection(Intersection *intersect,
                              const Intersection &new_intersect) {
  if (new_intersect.found) {
    // update intersection if the new one is closer
    if (new_intersect.t < intersect->t) {
      *intersect = new_intersect;
      return true;
    }
  }
  return false;
}

void BVH::print_node(bvh_node_pointer *node) {
  BVH_node_data *n = _data.tree.get_data(node);

  std::cout << "-------bvh_node------\n";
  std::cout << "count: " << n->triangle_ids.size() << "\n";
  std::cout << "min: " << glm::to_string(n->bounds.min) << "\n";
  std::cout << "max: " << glm::to_string(n->bounds.max) << "\n";
  std::cout << "leaf: " << _data.tree.is_leaf(node) << "\n";

  // triangle ids
  for (uint id : _data.tree.get_data(node)->triangle_ids) {
    std::cout << id << ",";
  }
  std::cout << "\n";
  std::cout << "----------------------\n";
}

void BVH::print_node_triangles(bvh_node_pointer *node) {
  std::cout << "-------bvh_node_triangles------\n";
  for (uint id : _data.tree.get_data(node)->triangle_ids) {
    Triangle *t = _data.triangles->data() + id;
    t->print();
  }
  std::cout << "----------------------\n";
}
