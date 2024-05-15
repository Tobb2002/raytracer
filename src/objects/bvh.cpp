/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "box.hpp"
#include <algorithm>
#include <boost/lambda/bind.hpp>
#include <cmath>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "bvh.hpp"


void BVH::build_tree_axis(std::vector<Triangle> *triangles) {
  // initialize data structure
  _data.triangles = triangles;

  // initialize tree with standart nodes
  _data.tree.reserve(_data.triangles->size());
  for (size_t i = 0; i < _data.triangles->size(); i++) {
    BVH_node node;
    _data.tree.push_back(node);
  }

  _data.size = _data.triangles->size();

  _data.triangle_ids.reserve(_data.size);
  for (size_t i = 0; i < _data.size; i++) {
    _data.triangle_ids.push_back(i);
  }
  // initialize root node
  _data.tree[0].count = _data.size;
  calculate_max(0);
  calculate_min(0);

  // split nodes recursivley
  split_SAH(0);
}

void BVH::set_triangles(std::vector<Triangle> *triangles) {
  _data.triangles = triangles;
}

Intersection BVH::intersect(const Ray& ray) {
  _intersect_count = 0;
  return intersect_node(0, ray);
}

/**
 * @brief recursivley calculate intersection in nodes
 * 
 * @param node_id 
 * @param ray 
 * @return Intersection 
 */
Intersection BVH::intersect_node(uint node_id, const Ray& ray) {
  Intersection result;
  if (!intersect_node_bool(node_id, ray)) {
    return result;
  }

  _intersect_count += 1;
  // check if leaf
  if (_data.tree[node_id].leaf) {
    Intersection intersect = intersect_leaf(node_id, ray);

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
bool BVH::intersect_node_bool(uint id, const Ray& ray) {
  vec3 d = ray.get_direction();
  vec3 o = ray.get_origin();

  vec3 box_min = _data.tree[id].min;
  vec3 box_max = _data.tree[id].max;

  Interval tx = {
    (box_min.x - o.x) / d.x,
    (box_max.x - o.x) / d.x};

  Interval ty = {
    (box_min.y - o.y) / d.y,
    (box_max.y - o.y) / d.y};

  Interval tz = {
    (box_min.z - o.z) / d.z,
    (box_max.z - o.z) / d.z};

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

Intersection BVH::intersect_leaf(uint id, const Ray& ray) {
  #ifdef DEBUG
  std::cout << "id: " << id << "  parent:" << (id -1) /2 << "\n";
  #endif
  // find best intersection in triangle set
  uint best_triangle_id = 0;
  float t_min = MAXFLOAT;


  uint i = _data.tree[id].first;
  for (; i < _data.tree[id].count + _data.tree[id].first; i++) {
    Intersection t_i =
      _data.triangles->at(_data.triangle_ids.at(i)).intersect(ray);

    if (t_i.found && t_i.t < t_min) {
      t_min = t_i.t;
      best_triangle_id = i;
    }
  }

  Triangle triangle_best =
    _data.triangles->at(_data.triangle_ids.at(best_triangle_id));

  return triangle_best.intersect(ray);
}

bool BVH::update_intersection(Intersection *intersect,
                              Intersection new_intersect) {
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

bvh_box BVH::update_box(uint node_id) {
  if (_data.tree[node_id].leaf) {
    calculate_max(node_id);
    calculate_min(node_id);
    return {_data.tree[node_id].min , _data.tree[node_id].max};
  }
  bvh_box left = update_box(node_id*2 +1);
  bvh_box right = update_box(node_id*2 +2);
  update_min(&left.min, right.min);
  update_max(&left.max, right.max);

  _data.tree[node_id].min = left.min;
  _data.tree[node_id].max = left.max;

  return {_data.tree[node_id].min , _data.tree[node_id].max};
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

void BVH::print_node(uint id) {
  BVH_node n = _data.tree[id];

  std::cout << "-------bvh_node------\n";
  std::cout << "id: " << id << "\n";
  std::cout << "first: " << n.first << "\n";
  std::cout << "count: " << n.count << "\n";
  std::cout << "min: " << glm::to_string(n.min) << "\n";
  std::cout << "max: " << glm::to_string(n.max) << "\n";
  std::cout << "leaf: " << n.leaf << "\n";

  // triangle ids
  for (size_t i = 0; i < _data.tree[id].count; i++) {
    std::cout << _data.triangle_ids.at(i + _data.tree[id].first) <<",";
  }
  std::cout << "\n";
  std::cout << "----------------------\n";
}

void BVH::print_node_triangles(uint id) {
  BVH_node n = _data.tree[id];

  std::cout << "-------bvh_node_triangles------\n";
  std::cout << "id: " << id << "\n";
  for (size_t i = 0; i < _data.tree[id].count; i++) {
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

bool comp(BVH_data *data, uint id1, uint id2, Axis axis) {
  if (data->triangles->at(id1).get_pos()[static_cast<size_t>(axis)]
    > data->triangles->at(id2).get_pos()[static_cast<size_t>(axis)]) {
      return true;
    }
  return false;
}

void BVH::sort(uint first, uint count, Axis axis) {
  namespace boo = boost::lambda;
  switch (axis) {
    case X:
      std::sort(
        _data.triangle_ids.begin() + first,
        _data.triangle_ids.begin() + first + count,
        boo::bind(&comp, &_data, boo::_1, boo::_2, axis));
      break;
    case Y:
      std::sort(
        _data.triangle_ids.begin() + first,
        _data.triangle_ids.begin() + first + count,
        boo::bind(&comp, &_data, boo::_1, boo::_2, axis));
      break;
    case Z:
      std::sort(
        _data.triangle_ids.begin() + first,
        _data.triangle_ids.begin() + first + count,
        boo::bind(&comp, &_data, boo::_1, boo::_2, axis));
      break;
  }
}

void BVH::split_middle(uint node_id) {
  // check if split is needed
  if (!(_data.tree[node_id].count > _max_triangles)) {
    return;
  }
  Axis axis = get_longest_axis(node_id);

  uint start = _data.tree[node_id].first;
  uint count = _data.tree[node_id].count;

  sort(start, count, axis);

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
  split_SAH(left_id);
  split_SAH(right_id);
}

vec3 calc_bucket_step(vec3 min, vec3 max) {
  vec3 res;
  for (size_t y = 0; y < 3; y++) {
    res[0] = (max[0] - min[0]) / SAH_NUM_BUCKETS;
  }
  return res;
}

void triangles_into_buckets(uint node_id, SAH_buckets *buckets, BVH_data *data) {
  vec3 min = data->tree.at(node_id).min;
  vec3 max = data->tree.at(node_id).max;

  uint start = data->tree[node_id].first;
  uint count = data->tree[node_id].count;

  // calculate bucket size (x,y,z)
  vec3 bucket_step = calc_bucket_step(min, max);

  // go trough all triangles in node
  for (size_t i = start; i < start + count; i++) {
    for (uint b = 0; b < SAH_NUM_BUCKETS; b++) {
      // get boundary for x,y,z-bucket[b] 
      vec3 split = min + static_cast<float>(b) * bucket_step;
      vec3 split_before;
      if (b != 0) {
        split_before = min + static_cast<float>(b - 1) * bucket_step;
      }
      else {
        split_before = min;
      }

      // check if triangle is in bucket b for x,y,z- bucket
      vec3 triangle_pos = data->triangles->at(data->triangle_ids.at(i)).get_pos();

      // triangle is in bucket if smaller than split if in bucket[i] it's also in all buckets[<i]
      for (size_t a = 0; a < 3; a++) {
        if (triangle_pos[a] < split[a] &&  triangle_pos[a] > split_before[a]) {
          // insert trianlge to specific bucket
          buckets->buckets[a][b].ids.push_back(i);
        }
      }
    }
  }

}

bvh_box BVH::combine_box(SAH_buckets *buckets, uint min, uint max) {
  // TODO
}

void BVH::calc_SAH_costs(uint node_id, SAH_buckets *buckets) {
  // go trough all buckets and generate split

  for (size_t a = 0; a < 3; a++) { // for every axis
    for (size_t b = 0; b < SAH_NUM_BUCKETS; b++) { // for every bucket
      bvh_box left = combine_box(buckets, 0,b);
      bvh_box right = combine_box(buckets, b, SAH_NUM_BUCKETS - 1);
    }
  }
}

void BVH::split_SAH(uint node_id) {
  // check if split needed -> triangles > 2
  if (!(_data.tree[node_id].count > _max_triangles)) {
    return;
  }

  SAH_buckets buckets;
  // sort triangles into buckets
  triangles_into_buckets(node_id, &buckets, &_data);

  // calculate costs for every split
  calc_SAH_costs(node_id, &buckets);

  // DEBUGING print bucket sizes

  /*
  printf("Triangles in node: %d\n", _data.tree[node_id].count);
  for (size_t i = 0; i <= SAH_NUM_BUCKETS; i++) {

  printf("content of bucket %zu:  ", i);
  printf("%zu\n", buckets.buckets[0][i].ids.size());
  }
  */


  // choose best split

  // split note
  
  split_middle(node_id);

}
