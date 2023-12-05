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

  std::cout << triangles->size() << "triangles\n";
  std::cout << _data.triangle_ids.size() << "triangle_ids\n";
  // split nodes recursivley
  split(0);
}

/**
 * @brief Return best triangle intersection if found.
 * 
 * @param ray 
 * @return Intersection 
 */
Intersection BVH::intersect(Ray *ray) {
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
  if (intersect_node_bool(node_id, ray)) {
    // check if leaf
    if (_data.tree[node_id].leaf) {
      result = intersect_leaf(node_id, ray);
    }
    else {  // check both leaves
      update_intersection(&result, intersect_node(node_id*2 +2, ray));
      update_intersection(&result, intersect_node(node_id*2 +1, ray));
    }
  }
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
  // find best intersection in triangle set
  uint best_triangle_id = 0;
  float t_min = MAXFLOAT;
  bool found_one = false;


  uint i = _data.tree[id].first;
  for (; i < _data.tree[id].count + _data.tree[id].first; i++) {
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

void BVH::update_intersection(Intersection *intersect, Intersection new_intersect) {
  if (new_intersect.found) {
    // update intersection if the new one is closer
    if (new_intersect.t < intersect->t) {
      *intersect = new_intersect;
    }
  }
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

void BVH::print_node_triangles(uint id) {
  BVH_node n = _data.tree[id];

  std::cout << "-------bvh_node_triangles------\n";
  std::cout << "id: " << id << "\n";
  for (int i = 0; i < _data.tree[id].count; i++) {
    Triangle t = _data.triangles->at(_data.triangle_ids.at(i + _data.tree[id].first));
    t.print();
  }
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
  print_node(node_id);
  // check if split is needed
  if (!(_data.tree[node_id].count > _max_triangles)) {
    return;
  }
  Axis axis = get_longest_axis(node_id);
  //sort(
  //  _data.tree[node_id].first,
  //  _data.tree[node_id].count,
  //  axis);

  // update node
  _data.tree[node_id].leaf = false;

  // point to split id belongs to the right child
  uint middle_count = _data.tree[node_id].count / 2;

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
  }
  else {  // asing left one more element
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
