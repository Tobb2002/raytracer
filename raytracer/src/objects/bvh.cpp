/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "bvh.hpp"

#include <algorithm>
#include <boost/lambda/bind.hpp>
#include <glm/gtx/string_cast.hpp>

//#define DEBUG

BVH::BVH() {
  //_data.size = 1;
  //_data.tree = new BVH_node[1];
}

//BVH::BVH(const BVH &bvh) {
//  //_data.triangles = bvh._data.triangles;
//  _data.triangle_ids = bvh._data.triangle_ids;
//  _data.size = bvh._data.size;
//
//  _data.tree = bvh._data.tree;
//
//  // allocate tree
//  //_data.tree = new BVH_node[_data.size];
//  //std::copy(bvh._data.tree, bvh._data.tree + bvh._data.size, _data.tree);
//}

BVH::~BVH(){
  //delete[] _data.tree;
}


void BVH::build_tree(std::vector<Triangle> *triangles) {
  // initialize data structure
  _data.triangles = triangles;
  //delete[] _data.tree;

  // initialize tree with standart nodes
  _data.tree.reserve(_data.triangles->size());
  for (int i = 0; i < _data.triangles->size(); i++) {
    BVH_node node;
    _data.tree.push_back(node);
  }

  for (int i = 0; i < _data.triangles->size(); i++) {
  }
  //_data.tree = new BVH_node[_data.triangles->size()];
  _data.size = _data.triangles->size();

  _data.triangle_ids.reserve(_data.size);
  for (int i = 0; i < _data.size; i++) {
    _data.triangle_ids.push_back(i);
  }
  // initialize root node
  _data.tree[0].count = _data.size;
  calculate_max(0);
  calculate_min(0);

  // split nodes recursivley
  split(0);
}
void BVH::set_triangles(std::vector<Triangle> *triangles) {
  _data.triangles = triangles;
}

/**
 * @brief Return best triangle intersection if found.
 * 
 * @param ray 
 * @return Intersection 
 */
Intersection BVH::intersect(Ray *ray) {

  _intersect_count = 0;

  // check root
  return intersect_node(0, ray);
}

/**
 * @brief recursivley calculate intersection in nodes
 * 
 * @param node_id 
 * @param ray 
 * @return Intersection 
 */
Intersection BVH::intersect_node(uint node_id, Ray *ray) {

  Intersection result;
  if (!intersect_node_bool(node_id, ray)) {
    return result;
  }

  _intersect_count += 1;
  // check if leaf
  if (_data.tree[node_id].leaf) {
  //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  //std::cout << "node_id: " << node_id << "\n";
  //std::cout << "node_size: " << _data.tree[node_id].count << "\n";
    Intersection intersect = intersect_leaf(node_id, ray);

  //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  //std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
    #ifdef DEBUG
    std::cout << "intersect_count: " << _intersect_count << "\n";
    #endif

    return intersect;
  }

  update_intersection(&result, intersect_node(node_id*2 +1, ray));
  update_intersection(&result, intersect_node(node_id*2 +2, ray));
  
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
bool BVH::intersect_node_bool(uint id, Ray *ray) {
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
  #ifdef DEBUG
  std::cout << "id: " << id << "  parent:" << (id -1) /2 << "\n";
  #endif
  // find best intersection in triangle set
  uint best_triangle_id = 0;
  float t_min = MAXFLOAT;


  uint i = _data.tree[id].first;
  for (; i < _data.tree[id].count + _data.tree[id].first; i++) {
    Intersection t_i = _data.triangles->at(_data.triangle_ids.at(i)).intersect(ray);

    if (t_i.found && t_i.t < t_min) {
      t_min = t_i.t;
      best_triangle_id = i;
    }
  }

  Triangle triangle_best = _data.triangles->at(_data.triangle_ids.at(best_triangle_id));
  //Intersection intersection = {found_one,
  //                             t_min,
  //                             ray->get_point(t_min),
  //                             triangle_best.get_normal(),
  //                             triangle_best.get_material()};

  return triangle_best.intersect(ray);
}

bool BVH::update_intersection(Intersection *intersect, Intersection new_intersect) {
  if (new_intersect.found) {
    // update intersection if the new one is closer
    if (new_intersect.t < intersect->t) {
      *intersect = new_intersect;
      return true;
    }
  }
  return false;
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

bvh_Box BVH::update_box(uint node_id) {
  if (_data.tree[node_id].leaf) {
    calculate_max(node_id);
    calculate_min(node_id);
    return {_data.tree[node_id].min ,_data.tree[node_id].max};
  }
  bvh_Box left = update_box(node_id*2 +1);
  bvh_Box right = update_box(node_id*2 +2);
  update_min(&left.min, right.min);
  update_max(&left.max, right.max);
  
  _data.tree[node_id].min = left.min;
  _data.tree[node_id].max = left.max;

  return {_data.tree[node_id].min ,_data.tree[node_id].max};
}

/**
 * @brief Goes throug all triangles in node and calculates min value;
 * 
 * @param node_id 
 * @return vec3 
 */
void BVH::calculate_min(uint node_id) {
  for (uint i = 0; i < _data.tree[node_id].count; i++) {
    uint index = i + _data.tree[node_id].first;
    Triangle t = _data.triangles->at(_data.triangle_ids.at(index));
    update_min(&_data.tree[node_id].min, t.get_min_bounding());
  }
}

void BVH::calculate_max(uint node_id) {
  for (uint i = 0; i < _data.tree[node_id].count; i++) {
    uint index = i + _data.tree[node_id].first;
    Triangle t = _data.triangles->at(_data.triangle_ids.at(index));
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
  std::cout << "posx: " << _data.triangles->at(_data.triangle_ids.at(id1)).get_pos()[static_cast<size_t>(axis)] << "\n";
  std::cout << "posx2: " << _data.triangles->at(_data.triangle_ids.at(id2)).get_pos()[static_cast<size_t>(axis)] << "\n";
  if (_data.triangles->at(_data.triangle_ids.at(id1)).get_pos()[static_cast<size_t>(axis)]
    < _data.triangles->at(_data.triangle_ids.at(id2)).get_pos()[static_cast<size_t>(axis)]) {
      return false;
    }
  return true;
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
  //triangle ids
  for (int i = 0; i < _data.tree[id].count; i++) {
    std::cout << _data.triangle_ids.at(i + _data.tree[id].first) <<",";
  }
  std::cout << "\n";
  std::cout << "----------------------\n";
}

void BVH::print_node_triangles(uint id) {
  BVH_node n = _data.tree[id];

  std::cout << "-------bvh_node_triangles------\n";
  std::cout << "id: " << id << "\n";
  for (int i = 0; i < _data.tree[id].count; i++) {
    Triangle t = 
      _data.triangles->at(_data.triangle_ids.at(i + _data.tree[id].first));
    t.print();
  }
  std::cout << "----------------------\n";
}

/**
 * @brief recursivley update the bounding boxes bottom up.
 * 
 */
void BVH::update_boxes() {
  update_box(0);
}

bool comp2(BVH_data *data, uint id1, uint id2, Axis axis)
{
  //std::cout << "id1: " << id1 << " id2: " << id2 << " |  ";
  if (data->triangles->at(id1).get_pos()[static_cast<size_t>(axis)]
    > data->triangles->at(id2).get_pos()[static_cast<size_t>(axis)]) {
      //std::cout << "false\n";
      return true;
    }
  //std::cout << "true\n";
  return false;
}

void BVH::sort(uint first, uint count, Axis axis) {
  //std::cout << _data.triangles->size() << "\n";
  //std::cout << "first: " << first << " count:" << count << "\n";
  //std::cout << "comp: " << comp2(&_data, 2,4, X) << "\n";
  namespace boo = boost::lambda;
  switch (axis) {
    case X:
      std::sort(
        _data.triangle_ids.begin() + first,
        _data.triangle_ids.begin() + first + count,
        boo::bind(&comp2, &_data, boo::_1, boo::_2, axis));
      break;
    case Y:
      std::sort(
        _data.triangle_ids.begin() + first,
        _data.triangle_ids.begin() + first + count,
        boo::bind(&comp2, &_data, boo::_1, boo::_2, axis));
      break;
    case Z:
      std::sort(
        _data.triangle_ids.begin() + first,
        _data.triangle_ids.begin() + first + count,
        boo::bind(&comp2, &_data, boo::_1, boo::_2, axis));
      break;
  }
}

void BVH::split(uint node_id) {
  // check if split is needed
  if (!(_data.tree[node_id].count > _max_triangles)) {
    return;
  }
  Axis axis = get_longest_axis(node_id);
  
  uint start = _data.tree[node_id].first;
  uint count = _data.tree[node_id].count;

  // std::cout << axis << "\n";
  // for (uint i : _data.triangle_ids) {
  //   std::cout << i << ",";
  // }
  // std::cout << "\n";
  // if ((start + count - 1 >= _data.size)) {
  //   print_node(node_id);
  //   std::cout << "hallooooooooo\n";
  // }
  sort(start, count, axis);
  // for (uint i : _data.triangle_ids) {
  //   std::cout << i << ",";
  // }
  // std::cout << "\n";

  // update node
  _data.tree[node_id].leaf = false;

  // point to split id belongs to the right child
  uint middle_count = floor(_data.tree[node_id].count / 2.f);

  bool even_amount = false;
  if (_data.tree[node_id].count % 2 == 0) {
    even_amount = true;
  }

  uint left_id = node_id*2 + 1;
  uint right_id = node_id*2 + 2;



  // if odd assing one more to left child
  if (even_amount) {
    // asing left child
    _data.tree[left_id].first = _data.tree[node_id].first;
    _data.tree[left_id].count = middle_count;

    // asing right child
    _data.tree[right_id].first =
      _data.tree[node_id].first + middle_count;
    _data.tree[right_id].count = middle_count;
  } else {  // asing left one more element
    // asing left child
    _data.tree[left_id].first = _data.tree[node_id].first;
    _data.tree[left_id].count = middle_count + 1;

    // asing right child
    _data.tree[right_id].first =
      _data.tree[node_id].first + middle_count + 1;
    _data.tree[right_id].count = middle_count;
  }

  // calculate new bounding boxes
  calculate_min(left_id);
  calculate_max(left_id);

  calculate_min(right_id);
  calculate_max(right_id);


  // recursivley split more
  split(left_id);
  split(right_id);
}
