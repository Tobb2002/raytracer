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

  uint size = _data.triangles->size() * 100;
  // initialize tree with standart nodes
  _data.tree.reserve(size);
  for (size_t i = 0; i < size; i++) {
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
Intersection BVH::intersect_node(const uint &node_id, const Ray& ray) {
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
bool BVH::intersect_node_bool(const uint &id, const Ray& ray) {
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

Intersection BVH::intersect_leaf(const uint &id, const Ray& ray) {
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

/// @brief swap Triangle ids in BVH_data.
void BVH::swap_triangle(const uint &id1, const uint &id2) {
  uint tmp = _data.triangle_ids[id1];
  _data.triangle_ids[id1] = _data.triangle_ids[id2];
  _data.triangle_ids[id2] = tmp;
}

Axis BVH::get_longest_axis(const uint &node_id) {
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

bvh_box BVH::update_box(const uint &node_id) {
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
void BVH::calculate_min(const uint &node_id) {
  for (uint i = 0; i < _data.tree[node_id].count; i++) {
    uint index = i + _data.tree[node_id].first;
    Triangle t = _data.triangles->at(_data.triangle_ids.at(index));
    update_min(&_data.tree[node_id].min, t.get_min_bounding());
  }
}

void BVH::calculate_max(const uint &node_id) {
  for (uint i = 0; i < _data.tree[node_id].count; i++) {
    uint index = i + _data.tree[node_id].first;
    Triangle t = _data.triangles->at(_data.triangle_ids.at(index));
    update_max(&_data.tree[node_id].max, t.get_max_bounding());
  }
}

void BVH::update_min(vec3 *min, const vec3 &min_value) {
  for (int i = 0; i < 3; i++) {
    // update min
    if (min_value[i] < (*min)[i]) {
      (*min)[i] = min_value[i];
    }
  }
}

void BVH::update_max(vec3 *max, const vec3 &max_value) {
  for (int i = 0; i < 3; i++) {
    // update min
    if (max_value[i] > (*max)[i]) {
      (*max)[i] = max_value[i];
    }
  }
}

void BVH::print_node(const uint &id) {
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

void BVH::print_node_triangles(const uint &id) {
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
    < data->triangles->at(id2).get_pos()[static_cast<size_t>(axis)]) {
      return true;
    }
  return false;
}

void BVH::sort(const uint &first, const uint &count, const Axis &axis) {
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

void BVH::split_middle(const uint &node_id) {
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
  split_middle(left_id);
  split_middle(right_id);
}

vec3 calc_bucket_step(vec3 min, vec3 max) {
  vec3 res;
  for (size_t a = 0; a < 3; a++) {
    res[a] = (max[a] - min[a]) / SAH_NUM_BUCKETS;
  }
  return res;
}

void BVH::triangles_into_buckets(uint node_id, SAH_buckets *buckets) {
  vec3 min = _data.tree.at(node_id).min;
  vec3 max = _data.tree.at(node_id).max;

  uint start = _data.tree[node_id].first;
  uint count = _data.tree[node_id].count;

  // calculate bucket size (x,y,z)
  vec3 bucket_step = calc_bucket_step(min, max);

  // go trough all triangles in node
  for (size_t i = start; i < start + count; i++) {
    // check if triangle is in bucket b for x,y,z- bucket
    vec3 triangle_pos = _data.triangles->at(_data.triangle_ids.at(i)).get_pos();

    for (uint b = 0; b < SAH_NUM_BUCKETS; b++) {
      // get boundary for x,y,z-bucket[b] 
      vec3 split_before = min + static_cast<float>(b) * bucket_step;
      vec3 split = min + static_cast<float>(b + 1) * bucket_step;

      // triangle is in bucket if smaller than split if in bucket[i] it's also in all buckets[<i]
      for (size_t a = 0; a < 3; a++) {
        if (triangle_pos[a] < split[a] &&  triangle_pos[a] >= split_before[a]) {
          // insert trianlge to specific bucket
          buckets->buckets[a][b].ids.push_back(i);
        }
      }
    }
  }

  // calcualte bucket bounding boxes
  for (size_t a = 0; a < 3; a++) {
    for (uint b = 0; b < SAH_NUM_BUCKETS; b++) {
      SAH_bucket bucket = buckets->buckets[a][b];
      buckets->buckets[a][b].box = bvh_box(vec3(MAXFLOAT), vec3(-MAXFLOAT));
      if (bucket.ids.size() <= 0) {
        continue;
      }

      // calculate bounding box
      for (size_t i = 0; i < bucket.ids.size(); i++) {
        Triangle t = _data.triangles->at(_data.triangle_ids.at(buckets->buckets[a][b].ids.at(i)));
        update_min(&buckets->buckets[a][b].box.min, t.get_min_bounding());
        update_max(&buckets->buckets[a][b].box.max, t.get_max_bounding());
      }
    }
  }

}

float BVH:: get_surface_area(const bvh_box& box) {
  return 2 * (box.max.x - box.min.x) + 
         2 * (box.max.y - box.min.y) +
         2 * (box.max.z - box.min.z);
}
float get_surface_area_axis(uint axis, const bvh_box& box) {
  return box.max[axis] - box.min[axis];
}

bvh_box BVH::combine_box(SAH_buckets *buckets, const uint &axis, const uint &bid_min, const uint &bid_max) {
  // TODO check what happens if no triangle in bucket
  bvh_box res = bvh_box(vec3(MAXFLOAT), vec3(-MAXFLOAT));
  for (size_t i = bid_min; i <= bid_max; i++) {
    // update bounding box only if there are triangles
    if (buckets->buckets[axis][i].ids.size() > 0) {
      update_min(&res.min, buckets->buckets[axis][i].box.min);
      update_max(&res.max, buckets->buckets[axis][i].box.max);
    }
  }
  return res;
}

split_point BVH::calc_min_split(const uint &node_id, SAH_buckets *buckets) {
  // go trough all buckets and generate split

  float min_cost = MAXFLOAT;
  split_point split;

  bvh_box box = bvh_box(_data.tree[node_id].min, _data.tree[node_id].max);
  float surface_box = get_surface_area(box);

  for (size_t a = 0; a < 3; a++) { // for every axis
    for (size_t split_id = 0; split_id < SAH_NUM_BUCKETS - 1; split_id++) { // for every bucket
      bvh_box left = combine_box(buckets, a, 0,split_id);
      bvh_box right = combine_box(buckets, a, split_id + 1, SAH_NUM_BUCKETS - 1);

      // calc probabilities that random ray hits box
      float prob_left = get_surface_area(left);
      float prob_right = get_surface_area(right);

      // get number of triangles
      uint left_amount = 0;
      for (size_t i = 0; i <= split_id; i++) {
        left_amount += buckets->buckets[a][i].ids.size();
      }

      uint right_amount = 0;
      for (size_t i = split_id + 1; i < SAH_NUM_BUCKETS; i++) {
        right_amount += buckets->buckets[a][i].ids.size();
      }

      // calculate costs
      float cost = MAXFLOAT - 1; // to ensure that split gets set at least one time
      if (!(left_amount == 0 || right_amount == 0)) {
      cost = COST_TRAVERSAL +
                   prob_left * left_amount * COST_INTERSECT +
                   prob_right * right_amount * COST_INTERSECT;
      }

      buckets->buckets[a][split_id].cost = cost;

      printf("axis: %zu \t bucket: %zu \t num_bucket:%zu \t cost: %f\n", a, split_id, buckets->buckets[a][split_id].ids.size(), cost);
      //printf("bound_left: min:%f \t max:%f\n amount:%i\n", left.min[a], left.max[a], left_amount);
      //printf("bound_right: min:%f \t max:%f amount:%i\n", right.min[a], right.max[a], right_amount);

      // update min cost
      if (cost < min_cost) {
        min_cost = cost;
        split.left_count = left_amount;
        split.axis = a;
        split.left = left;
        split.right = right;
      }
    }
  }
  return split;
}

/**
 * @brief 
 *
 * @param node_id 
 * @param axis which to split
 * @param distance beetween split and min of bounding box in given axis
 */
void BVH::split(const uint &node_id, const size_t &axis, const uint &left_amount) {
  if (_data.tree[node_id].count < _max_triangles) {
    return;
  }
  uint start = _data.tree[node_id].first;
  uint count = _data.tree[node_id].count;

  Axis ax = X;
  if (axis == 0) { ax = X; };
  if (axis == 1) { ax = Y; };
  if (axis == 2) { ax = Z; };

  
  sort(start, count, ax);

  // Split node at triangle with split id
  // update node
  _data.tree[node_id].leaf = false;



  // asing child nodes 
  uint left_id = node_id*2 + 1;
  uint right_id = node_id*2 + 2;

  // asing left child
  _data.tree[left_id].first = start;
  _data.tree[left_id].count = left_amount;

  // asing right child
  _data.tree[right_id].first = start + left_amount;
  _data.tree[right_id].count = _data.tree[node_id].count - _data.tree[left_id].count;
}

void BVH::split_SAH(const uint &node_id) {
  // check if split needed -> triangles > 2
  if (_data.tree[node_id].count < _max_triangles) {
    return;
  }

  SAH_buckets buckets;
  // sort triangles into buckets
  triangles_into_buckets(node_id, &buckets);

  // calculate costs for every split
  split_point splitp = calc_min_split(node_id, &buckets);

  printf("node id: %i \t count: %i\n", node_id, _data.tree[node_id].count);
  printf("Best split: %zu, %zu\n", splitp.axis, splitp.left_count);
  //printf("node cound: %i \n", _data.tree[node_id].count );
  
  // split the node
  float bucket_width = (_data.tree[node_id].max[splitp.axis] - _data.tree[node_id].min[splitp.axis]) / SAH_NUM_BUCKETS;
  float distance = (splitp.left_count + 1) * bucket_width; 

  split(node_id, splitp.axis, splitp.left_count);

  // set bounding boxes
  size_t left = node_id *2 + 1;
  size_t right = node_id *2 + 2;

  _data.tree[left].min = splitp.left.min;
  _data.tree[left].max = splitp.left.max;

  _data.tree[right].min = splitp.right.min;
  _data.tree[right].max = splitp.right.max;

  // recursivley continue splitting
  split_SAH(left);
  split_SAH(right);
}
