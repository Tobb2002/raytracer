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

void BVH::build_tree_axis(std::vector<Triangle> *triangles,
                          Algorithm algorithm) {
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

  SAH sah = SAH(&_data.tree);
  LBVH lbvh = LBVH(&_data.tree);

  switch (algorithm) {
    case AMID:
      std::cout << "Algorithm: Split middle\n";
      sah.split_middle(root);
      break;
    case ASAH:
      std::cout << "Algorithm: SAH\n";
      sah.split(root);
      break;
    case ALBVH:
      std::cout << "Algorithm: LBVH\n";
      lbvh = LBVH(&_data.tree);
      // lbvh.sort();
      lbvh.build();
      break;
    case AHLBVH:
      std::cout << "Algorithm: HLBVH\n";
      lbvh = LBVH(&_data.tree);

      lbvh.build_treelets();
      sah.built_on_treelets();
      break;
  }

#if FLATTEN_TREE
  _data.tree.flatten_tree();
#endif
}

void BVH::set_triangles(std::vector<Triangle> *triangles) {
  _data.triangles = triangles;
  _data.tree.set_triangles(triangles);
}

Intersection BVH::intersect(const Ray &ray) {
  _intersect_count = 0;
  _stats = bvh_stats();
  _best_intersection = Intersection();
#if FLATTEN_TREE
  intersect_node((uint)0, ray);
  return _best_intersection;
#else
  intersect_node(_data.tree.get_root(), ray);
  return _best_intersection;
#endif
}

/**
 * @brief recursivley calculate intersection in nodes
 *
 * @param node_id
 * @param ray
 * @return Intersection
 */
void BVH::intersect_node(bvh_node_pointer *node, const Ray &ray) {
  if (!intersect_node_bool(_data.tree.get_data(node), ray)) {
    return;
  }

  // check if leaf
  if (_data.tree.is_leaf(node)) {
    return intersect_leaf(_data.tree.get_data(node), ray);
  }

  if (ray.get_direction()[node->data.axis] > 0) {
    intersect_node(_data.tree.get_left(node), ray);
    intersect_node(_data.tree.get_right(node), ray);
  } else {
    intersect_node(_data.tree.get_right(node), ray);
    intersect_node(_data.tree.get_left(node), ray);
  }
}

void BVH::intersect_node(uint id_flat, const Ray &ray) {
  BVH_node_data *data = _data.tree.get_data(id_flat);
  if (!intersect_node_bool(data, ray)) {
    return;
  }

  // check if leaf
  if (_data.tree.get_node(id_flat)->is_leaf) {
    intersect_leaf(data, ray);
    return;
  }

  if (ray.get_direction()[data->axis] > 0) {
    intersect_node(id_flat + 1, ray);
    intersect_node(_data.tree.get_right(id_flat), ray);
  } else {
    intersect_node(_data.tree.get_right(id_flat), ray);
    intersect_node(id_flat + 1, ray);
  }
}
/**
 * @brief check if hitbox of node has intersection. that is closer than t of
 * _current_best.
 *
 * @param id
 * @param ray
 * @return true
 * @return false
 */
bool BVH::intersect_node_bool(BVH_node_data *node_data, const Ray &ray) {
  _intersect_count += 1;
#if GET_STATS
  _stats.node_intersects += 1;
#endif
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

  // discard if _best_intersection is closer than bounding box
  if (_best_intersection.t <= tx.min || _best_intersection.t <= ty.min ||
      _best_intersection.t <= tz.min) {
    return false;
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

void BVH::intersect_leaf(BVH_node_data *node_data, const Ray &ray) {
#ifdef DEBUG
  std::cout << "id: " << id << "  parent:" << (id - 1) / 2 << "\n";
#endif
  // find best intersection in triangle set
  uint best_triangle_id = 0;
  float t_min = MAXFLOAT;

  for (uint i : node_data->triangle_ids) {
    Intersection t_i = _data.triangles->at(i).intersect(ray);
#if GET_STATS
    _stats.triangle_intersects += 1;
#endif

    if (t_i.found && t_i.t < t_min) {
      t_min = t_i.t;
      best_triangle_id = i;
    }
  }

  Intersection res =
      (_data.triangles->data() + best_triangle_id)->intersect(ray);

#if VISUALIZE_INTERSECT
  vec2 input_area = VISUALIZE_RANGE;
  vec2 output_area = vec2(0, 1);

  float value = (_intersect_count - input_area.x) *
                    (output_area.y - output_area.x) /
                    (input_area.y - input_area.x) +
                output_area.x;
  res.material.color.x = value;
  res.material.color.y = value;
  res.material.color.z = value;
#endif
  update_intersection(&_best_intersection, res);
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

bvh_stats BVH::get_stats() { return _stats; }
