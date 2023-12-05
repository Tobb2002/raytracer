/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "bvh.hpp"

#include <algorithm>
#include <boost/lambda/bind.hpp>
#include <glm/gtx/string_cast.hpp>

BVH::BVH() {
}

BVH::~BVH(){
  delete[] _data.tree;
}

void BVH::build_tree(std::vector<Triangle> *triangles) {
  // initialize data structure
  _data.triangles = triangles;
  _data.tree = new BVH_node[_data.triangles->size()];
  _data.size = _data.triangles->size();

  _data.triangle_ids.reserve(_data.size);
  for (int i = 0; i < _data.size; i++) {
    _data.triangle_ids.push_back(i);
  }
  // initialize root node
  _data.tree[0].count = _data.size;
  calculate_max(0);
  calculate_min(0);
  print_node(0);
}

/**
 * @brief Return best triangle intersection if found.
 * 
 * @param ray 
 * @return Intersection 
 */
Intersection BVH::intersect(Ray *ray) {
  // check root
  uint node_id = 0;
  // if leaf -> update best intersection

  // else check childs
  //if (intersect_node(node_id, ray)) {

  //}
  Intersection result;
  if (intersect_node(node_id, ray)) {
    result = intersect_leaf(node_id, ray);
  }
  return result;
}


bool BVH::intersect_node(uint id, Ray *ray) {
  Interval tx = {(_data.tree[id].min.x - ray->get_origin().x) / ray->get_direction().x,
                (_data.tree[id].max.x - ray->get_origin().x) / ray->get_direction().x};

  Interval ty = {(_data.tree[id].min.y - ray->get_origin().y) / ray->get_direction().y,
                (_data.tree[id].max.y - ray->get_origin().y) / ray->get_direction().y};

  Interval tz = {(_data.tree[id].min.z - ray->get_origin().z) / ray->get_direction().z,
                (_data.tree[id].max.z - ray->get_origin().z) / ray->get_direction().z};

  // overlapping intervals indicate intersection
  // check box intersection

  if (tx.min > tx.max) {
    std::swap(tx.min, tx.max);
  }
  if (ty.min > ty.max) {
    std::swap(ty.min, ty.max);
  }
  if (tz.min > tz.max) {
    std::swap(tz.min, tz.max);
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

Intersection BVH::intersect_leaf(uint id, Ray *ray) {
  // find best intersection in triangle set
  uint best_triangle_id = 0;
  float t_min = MAXFLOAT;
  bool found_one = false;


  uint i = _data.tree[id].first;
  for (; i < _data.tree[id].count; i++) {
    Intersection t_i = _data.triangles->at(_data.triangle_ids.at(i)).intersect(ray);

    if (t_i.found && t_i.t < t_min) {
      found_one = true;
      t_min = t_i.t;
      best_triangle_id = i;
    }
  }

  Triangle triangle_best = _data.triangles->at(best_triangle_id);
  Intersection intersection = {found_one,
                               t_min,
                               ray->get_point(t_min),
                               triangle_best.get_normal(),
                               triangle_best.get_material()};

  return intersection;
}

/// @brief swap Triangle ids in BVH_data.
void BVH::swap_triangle(uint id1, uint id2) {
  uint tmp = _data.triangle_ids[id1];
  _data.triangle_ids[id1] = _data.triangle_ids[id2];
  _data.triangle_ids[id2] = tmp;
}

Axis BVH::get_longest_axis(uint node_id) {
  float longest = 0;
  Axis res = X;
  for (int i = 0; i < 3; i++) {
    float length = _data.tree[node_id].max[i] - _data.tree[node_id].min[i];
    if (length > longest) {
      longest = length;
      res = static_cast<Axis>(i);
    }
  }
  return res;
}

/**
 * @brief Goes throug all triangles in node and calculates min value;
 * 
 * @param node_id 
 * @return vec3 
 */
void BVH::calculate_min(uint node_id) {
  uint i = _data.tree[node_id].first;
  for (; i < _data.tree[node_id].count; i++) {
    Triangle t = _data.triangles->at(_data.triangle_ids.at(i));
    update_min(&_data.tree[node_id].min, t.get_min_bounding());
  }
}

void BVH::calculate_max(uint node_id) {
  uint i = _data.tree[node_id].first;
  for (; i < _data.tree[node_id].count; i++) {
    Triangle t = _data.triangles->at(_data.triangle_ids.at(i));
    update_max(&_data.tree[node_id].max, t.get_max_bounding());
  }
}

void BVH::update_min(vec3 *min, vec3 min_value) {
  for (int i = 0; i < 3; i++) {
    // update min
    if (min_value[i] < (*min)[i]) {
      (*min)[i] = min_value[i];
    }
  }
}

void BVH::update_max(vec3 *max, vec3 max_value) {
  for (int i = 0; i < 3; i++) {
    // update min
    if (max_value[i] > (*max)[i]) {
      (*max)[i] = max_value[i];
    }
  }
}

bool BVH::comp(uint id1, uint id2, Axis axis) {
  if (_data.triangles->at(_data.triangle_ids.at(id1)).get_pos()[static_cast<size_t>(axis)]
    < _data.triangles->at(_data.triangle_ids.at(id2)).get_pos()[static_cast<size_t>(axis)]) {
      return true;
    }
  return false;
}
void BVH::print_node(uint id) {
  BVH_node n = _data.tree[id];

  std::cout << "-------bvh_node------\n";
  std::cout << "id: " << id << "\n";
  std::cout << "first: " << n.first << "\n";
  std::cout << "count: " << n.count << "\n";
  std::cout << "min: " << glm::to_string(n.min) << "\n";
  std::cout << "max: " << glm::to_string(n.max) << "\n";
  std::cout << "leaf: " << n.leaf << "\n";
  std::cout << "----------------------\n";
}
bool comp2(BVH_data *data, uint id1, uint id2, Axis axis)
{
  if (data->triangles->at(data->triangle_ids.at(id1)).get_pos()[static_cast<size_t>(axis)]
    < data->triangles->at(data->triangle_ids.at(id2)).get_pos()[static_cast<size_t>(axis)]) {
      return true;
    }
  return false;
}

void BVH::sort(uint first, uint count, Axis axis) {
  namespace boo = boost::lambda;
  auto comp_x = boost::lambda::bind(&comp2, &_data, boo::_1, boo::_1, X);
  auto comp_y = boo::bind(&comp2, &_data, boo::_1, boo::_1, Y);
  auto comp_z = boo::bind(&comp2, &_data, boo::_1, boo::_1, Z);
  switch (axis) {
    case X:
      std::sort(
        _data.triangle_ids.begin() + first,
        _data.triangle_ids.begin() + first + count,
        comp_x);
      break;
    case Y:
      std::sort(
        _data.triangle_ids.begin() + first,
        _data.triangle_ids.begin() + first + count,
        comp_y);
      break;
    case Z:
      std::sort(
        _data.triangle_ids.begin() + first,
        _data.triangle_ids.begin() + first + count,
        comp_z);
      break;
  }
}

void BVH::split(uint node_id) {
  Axis axis = get_longest_axis(node_id);
  sort(
    _data.tree[node_id].first,
    _data.tree[node_id].count,
    axis);

  // update node
  _data.tree[node_id].leaf = false;

  // point to split id belongs to the right child
  uint split_point = _data.tree[node_id].count / 2;
  
  // asing left child
  uint left_id = node_id*2 + 1;

  _data.tree[left_id].first = _data.tree[node_id].first;
  _data.tree[left_id].count = split_point;

  calculate_min(left_id);
  calculate_max(left_id);

  // if initialized corretly not nessecary
  _data.tree[left_id].leaf = true;

  // asing right child
  uint right_id = node_id*2 + 2;

  _data.tree[right_id].first =
    _data.tree[node_id].first + split_point;
  _data.tree[right_id].count = 
    _data.tree[node_id].count - split_point;

  calculate_min(right_id);
  calculate_max(right_id);

  _data.tree[right_id].leaf = true;

  // recursivley split more
  if (_data.tree[left_id].count > _max_triangles) {
    _data.tree[left_id].leaf = false;
    split(left_id);
  }
  if (_data.tree[right_id].count > _max_triangles) {
    _data.tree[right_id].leaf = false;
    split(right_id);
  }
}
