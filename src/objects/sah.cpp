/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "sah.hpp"

#include <algorithm>
#include <boost/lambda/bind.hpp>
#include <cmath>

#include "bvh_tree.hpp"
#include "lbvh.hpp"

SAH::SAH(BVH_tree *tree) { _tree = tree; }

// ---------------------------------------------------------------------------------
// ----- sorting -----

bool comp(BVH_tree *tree, uint id1, uint id2, uint axis) {
  if (tree->get_triangle(id1)->get_pos()[axis] <
      tree->get_triangle(id2)->get_pos()[axis]) {
    return true;
  }
  return false;
}

void SAH::sort(std::vector<uint>::iterator begin, const uint &count,
               const uint &axis) {
  namespace boo = boost::lambda;
  std::sort(begin, begin + count,
            boo::bind(&comp, _tree, boo::_1, boo::_2, axis));
}

bool comp_treelets(BVH_tree *tree, uint id1, uint id2, uint axis) {
  bvh_box middle1 = tree->get_treelet(id1)->data.bounds;
  bvh_box middle2 = tree->get_treelet(id2)->data.bounds;
  if (tree->get_middle(middle1)[axis] < tree->get_middle(middle2)[axis]) {
    return true;
  }
  return false;
}

void SAH::sort_treelets(std::vector<uint>::iterator begin, const uint &count,
                        const uint &axis) {
  namespace boo = boost::lambda;
  std::sort(begin, begin + count,
            boo::bind(&comp_treelets, _tree, boo::_1, boo::_2, axis));
}

// -----------------------------------------------------------------------------

vec3 calc_bucket_step(vec3 min, vec3 max) {
  vec3 res;
  for (size_t a = 0; a < 3; a++) {
    res[a] = (max[a] - min[a]) / SAH_NUM_BUCKETS;
  }
  return res;
}

float SAH::get_surface_area(const bvh_box &box) {
  return 2 * (box.max.x - box.min.x) + 2 * (box.max.y - box.min.y) +
         2 * (box.max.z - box.min.z);
}
float get_surface_area_axis(uint axis, const bvh_box &box) {
  return box.max[axis] - box.min[axis];
}

// -----------------------------------------------------------------------------
// Filling buckets

void SAH::triangles_into_buckets(bvh_node_pointer *node, SAH_buckets *buckets) {
  vec3 min = _tree->get_data(node)->bounds.min;
  vec3 max = _tree->get_data(node)->bounds.max;

  // calculate bucket size (x,y,z)
  vec3 bucket_step = calc_bucket_step(min, max);

  vec3 length = max - min;
  // go trough all triangles in node
  for (uint i : _tree->get_data(node)->triangle_ids) {
    // check if triangle is in bucket b for x,y,z- bucket
    Triangle *triangle = _tree->get_triangle(i);
    vec3 triangle_pos = triangle->get_pos();
    vec3 length_to_pos = triangle_pos - min;

    for (size_t a = 0; a < 3; a++) {
      uint b_id = SAH_NUM_BUCKETS - 1;
      if (length[a] > 0.00001) {  // values smaller than 1e-5 are eual to zero
        b_id = length_to_pos[a] / length[a] * (SAH_NUM_BUCKETS - 1);
      }
      buckets->buckets[a][b_id].ids.push_back(i);
      // update bounds
      buckets->buckets[a][b_id].box = union_box(
          buckets->buckets[a][b_id].box,
          bvh_box(triangle->get_min_bounding(), triangle->get_max_bounding()));
    }
  }
}

uint SAH::triangles_into_buckets_axis(bvh_node_pointer *node,
                                      SAH_buckets *buckets) {
  vec3 min = _tree->get_data(node)->bounds.min;
  vec3 max = _tree->get_data(node)->bounds.max;

  // calculate bucket size (x,y,z)
  vec3 length = max - min;

  uint a = _tree->get_longest_axis(node);

  // go trough all triangles in node
  for (uint i : _tree->get_data(node)->triangle_ids) {
    // check if triangle is in bucket b for x,y,z- bucket
    vec3 triangle_pos = _tree->get_triangle(i)->get_pos();
    vec3 length_to_pos = triangle_pos - min;

    uint b_id = 0;
    if (length[a] > 0.00001) {
      b_id = (length_to_pos[a] / length[a]) * (SAH_NUM_BUCKETS - 1);
    }
    buckets->buckets[0][b_id].ids.push_back(i);
  }

  // calcualte bucket bounding boxes
  for (uint b = 0; b < SAH_NUM_BUCKETS; b++) {
    buckets->buckets[0][b].box = bvh_box(vec3(MAXFLOAT), vec3(-MAXFLOAT));

    // calculate bounding box
    for (uint i : buckets->buckets[0][b].ids) {
      Triangle *t = _tree->get_triangle(i);
      _tree->update_bounds(
          &buckets->buckets[0][b].box.min, t->get_min_bounding(),
          &buckets->buckets[0][b].box.max, t->get_max_bounding());
    }
  }
  return a;
}

void SAH::treelets_into_buckets(bvh_node_pointer *node, SAH_buckets *buckets) {
  // bvh_box box = combine_box_treelets(node);
  bvh_box box = node->data.bounds;

  vec3 min = box.min;
  vec3 max = box.max;

  // calculate bucket size (x,y,z)
  vec3 length = max - min;

  // go trough all triangles in node
  for (uint id : node->data.triangle_ids) {
    // check if triangle is in bucket b for x,y,z- bucket
    bvh_node_pointer *treelet = _tree->get_treelet(id);
    vec3 treelet_pos = _tree->get_middle(treelet->data.bounds);

    vec3 length_to_pos = treelet_pos - min;

    for (size_t a = 0; a < 3; a++) {
      uint b_id = 0;
      if (length[a] > 0.00001) {
        b_id = (length_to_pos[a] / length[a]) * (SAH_NUM_BUCKETS - 1);
      }
      buckets->buckets[a][b_id].ids.push_back(id);
      // update bounds
      buckets->buckets[a][b_id].box =
          union_box(buckets->buckets[a][b_id].box, treelet->data.bounds);
    }
  }
}

uint SAH::treelets_into_buckets_axis(bvh_node_pointer *node,
                                     SAH_buckets *buckets) {
  bvh_box box = combine_box_treelets(node);
  vec3 min = box.min;
  vec3 max = box.max;

  // calculate bucket size (x,y,z)
  vec3 bucket_step = calc_bucket_step(min, max);
  vec3 length = max - min;

  uint a = _tree->get_longest_axis(node);

  // go trough all triangles in node
  for (uint id : node->data.triangle_ids) {
    // check if triangle is in bucket b for x,y,z- bucket
    bvh_node_pointer *treelet = _tree->get_treelet(id);
    vec3 treelet_pos = _tree->get_middle(treelet->data.bounds);
    vec3 length_to_pos = treelet_pos - min;

    uint b_id = 0;
    if (length[a] > 0.00001) {
      b_id = (length_to_pos[a] / length[a]) * (SAH_NUM_BUCKETS - 1);
    }
    buckets->buckets[0][b_id].ids.push_back(id);
    // update bounds
    buckets->buckets[0][b_id].box =
        union_box(buckets->buckets[0][b_id].box, treelet->data.bounds);
  }
  return a;
}

// -----------------------------------------------------------------------------
// Combine bounding boxes

bvh_box SAH::combine_box(const SAH_buckets &buckets, const uint &axis,
                         const uint &bid_min, const uint &bid_max) {
  bvh_box res;
  for (size_t i = bid_min; i <= bid_max; i++) {
    // update bounding box only if there are triangles
    if (buckets.buckets[axis][i].ids.size() > 0) {
      _tree->update_min(&res.min, buckets.buckets[axis][i].box.min);
      _tree->update_max(&res.max, buckets.buckets[axis][i].box.max);
    }
  }
  return res;
}

bvh_box SAH::combine_box_treelets(bvh_node_pointer *node) {
  bvh_box res = bvh_box(vec3(MAXFLOAT), vec3(-MAXFLOAT));
  if (node->data.triangle_ids.size() <= 0) {
    return res;
  }
  std::vector<bvh_node_pointer *> treelets = _tree->get_treelets();
  for (uint id : node->data.triangle_ids) {
    bvh_box current_box = treelets.at(id)->data.bounds;
    _tree->update_bounds(&res.min, current_box.min, &res.max, current_box.max);
  }
  return res;
}

void SAH::combine_ids(std::vector<uint> *res, const SAH_buckets &buckets,
                      const uint &axis, const uint &bid_min,
                      const uint &bid_max) {
  for (size_t i = bid_min; i <= bid_max; i++) {
    for (uint id : buckets.buckets[axis][i].ids) {
      res->push_back(id);
    }
  }
}

bvh_box SAH::union_box(bvh_box a, bvh_box b) {
  return bvh_box(vec3(std::min(a.min.x, b.min.x), std::min(a.min.y, b.min.y),
                      std::min(a.min.z, b.min.z)),
                 vec3(std::max(a.max.x, b.max.x), std::max(a.max.y, b.max.y),
                      std::max(a.max.z, b.max.z)));
}

// -----------------------------------------------------------------------------
// Find best possible split

split_point SAH::calc_min_split(bvh_node_pointer *node, SAH_buckets *buckets) {
  // go trough all buckets and generate split

  // bvh_box box = bvh_box(_tree->get_data(node)->bounds.min,
  //                       _tree->get_data(node)->bounds.max);
  // float surface_box = get_surface_area(box);
  //
  float cost[3][SAH_NUM_BUCKETS - 1] = {};

  for (size_t a = 0; a < 3; a++) {  // for every axis
    bvh_box left_bounds;
    bvh_box right_bounds;

    uint left_amount = 0;
    uint right_amount = 0;

    for (size_t i = 0; i < SAH_NUM_BUCKETS - 1; i++) {
      left_bounds = union_box(left_bounds, buckets->buckets[a][i].box);
      left_amount += buckets->buckets[a][i].ids.size();
      // partially initialize costs
      cost[a][i] +=
          get_surface_area(left_bounds) * left_amount * COST_INTERSECT;
    }

    for (uint i = SAH_NUM_BUCKETS - 1; i > 0; i--) {
      right_bounds = union_box(right_bounds, buckets->buckets[a][i].box);
      right_amount += buckets->buckets[a][i].ids.size();
      // add costs of right child
      cost[a][i - 1] +=
          get_surface_area(right_bounds) * right_amount * COST_INTERSECT;
    }
  }
  // calculate minimum split
  float min_cost = MAXFLOAT;
  split_point split;

  bool run_trough = true;

  for (size_t a = 0; a < 3; a++) {
    for (size_t split_id = 0; split_id < SAH_NUM_BUCKETS - 1; split_id++) {
      float cost_split = COST_TRAVERSAL + cost[a][split_id];
      if (cost_split < min_cost) {
        min_cost = cost_split;
        split.id = split_id;
        split.axis = a;
        run_trough = false;
      }
    }
  }
  if (run_trough) {
    split.axis = 3;  // Fallback to middle split
  }
  // check if leave is less costly
  float cost_leave = COST_TRAVERSAL + get_surface_area(node->data.bounds) *
                                          node->data.triangle_ids.size() *
                                          COST_INTERSECT;
  if (cost_leave < min_cost) {
    split.axis = 4;  // Do not further split
  }

  // std::cout << "best split: " << split.id << " " << split.axis << "\n";
  return split;
}

split_point SAH::calc_min_split(bvh_node_pointer *node, SAH_buckets *buckets,
                                uint axis) {
  // go trough all buckets and generate split
  float cost[SAH_NUM_BUCKETS - 1] = {};

  bvh_box left_bounds;
  bvh_box right_bounds;

  // get number of triangles
  uint left_amount = 0;
  uint right_amount = 0;

  for (size_t i = 0; i < SAH_NUM_BUCKETS - 1; i++) {
    left_bounds = union_box(left_bounds, buckets->buckets[0][i].box);
    left_amount += buckets->buckets[0][i].ids.size();
    // partially initialize costs
    cost[i] += get_surface_area(left_bounds) * left_amount * COST_INTERSECT;
  }

  for (uint i = SAH_NUM_BUCKETS - 1; i > 0; i--) {
    right_bounds = union_box(right_bounds, buckets->buckets[0][i].box);
    right_amount += buckets->buckets[0][i].ids.size();
    // add costs of right child
    cost[i - 1] +=
        get_surface_area(right_bounds) * right_amount * COST_INTERSECT;
  }

  // calculate minimum split
  float min_cost = MAXFLOAT;
  split_point split;

  bool run_trough = true;

  for (size_t split_id = 0; split_id < SAH_NUM_BUCKETS - 1; split_id++) {
    float cost_split = COST_TRAVERSAL + cost[split_id];
    if (cost_split < min_cost) {
      min_cost = cost_split;
      split.id = split_id;
      split.axis = axis;
      run_trough = false;
    }
  }
  if (run_trough) {
    split.axis = 3;  // Fallback to middle split
  }
  // check if leave is less costly
  float cost_leave = COST_TRAVERSAL + get_surface_area(node->data.bounds) *
                                          node->data.triangle_ids.size() *
                                          COST_INTERSECT;
  if (cost_leave < min_cost) {
    // split.axis = 4;  // Do not further split
  }
  return split;
}

split_point SAH::calc_min_split_treelets(bvh_node_pointer *node,
                                         SAH_buckets *buckets) {
  // go trough all buckets and generate split

  float cost[3][SAH_NUM_BUCKETS - 1] = {};
  // sort treelets respective to axis

  for (size_t a = 0; a < 3; a++) {  // for every axis
    bvh_box left_bounds;
    bvh_box right_bounds;

    // get number of triangles
    uint left_amount = 0;
    uint right_amount = 0;

    for (size_t i = 0; i < SAH_NUM_BUCKETS - 1; i++) {
      left_bounds = union_box(left_bounds, buckets->buckets[a][i].box);
      for (uint id : buckets->buckets[a][i].ids) {
        left_amount += _tree->get_treelet(id)->data.triangle_ids.size();
      }
      cost[a][i] +=
          get_surface_area(left_bounds) * left_amount * COST_INTERSECT;
    }

    for (uint i = SAH_NUM_BUCKETS - 1; i > 0; i--) {
      right_bounds = union_box(right_bounds, buckets->buckets[a][i].box);
      for (uint id : buckets->buckets[a][i].ids) {
        right_amount += _tree->get_treelet(id)->data.triangle_ids.size();
      }
      cost[a][i - 1] +=
          get_surface_area(right_bounds) * right_amount * COST_INTERSECT;
    }
  }

  // calculate minimum split
  float min_cost = MAXFLOAT;
  split_point split;

  bool run_trough = true;

  for (size_t a = 0; a < 3; a++) {
    for (size_t split_id = 0; split_id < SAH_NUM_BUCKETS - 1; split_id++) {
      float cost_split = COST_TRAVERSAL + cost[a][split_id];
      if (cost_split < min_cost) {
        min_cost = cost_split;
        split.id = split_id;
        split.axis = a;
        run_trough = false;
      }
    }
  }
  if (run_trough) {
    split.axis = 3;  // Fallback to middle split
  }
  // std::cout << "best_split: " << split.axis << " : " << split.id << "\n";
  return split;
}

split_point SAH::calc_min_split_treelets(bvh_node_pointer *node,
                                         SAH_buckets *buckets, uint axis) {
  // go trough all buckets and generate split

  float min_cost = MAXFLOAT;
  split_point split;

  // bvh_box box = combine_box_treelets(node);
  bvh_box box = node->data.bounds;
  float surface_box = get_surface_area(box);

  // sort treelets respective to axis
  bool run_trough = true;
  for (size_t split_id = 0; split_id < SAH_NUM_BUCKETS - 1;
       split_id++) {  // for every bucket
    bvh_box left;
    bvh_box right;

    // get number of triangles
    uint left_amount = 0;
    for (size_t i = 0; i <= split_id; i++) {
      left = union_box(left, buckets->buckets[0][i].box);
      for (uint id : buckets->buckets[0][i].ids) {
        left_amount += _tree->get_treelet(id)->data.triangle_ids.size();
      }
    }

    uint right_amount = 0;
    for (size_t i = split_id + 1; i < SAH_NUM_BUCKETS; i++) {
      right = union_box(right, buckets->buckets[0][i].box);
      for (uint id : buckets->buckets[0][i].ids) {
        right_amount += _tree->get_treelet(id)->data.triangle_ids.size();
      }
    }

    // calc probabilities that random ray hits box
    float prob_left = get_surface_area(left);
    float prob_right = get_surface_area(right);

    // calculate costs
    float cost = COST_TRAVERSAL + prob_left * left_amount * COST_INTERSECT +
                 prob_right * right_amount * COST_INTERSECT;

    // update min cost
    // gets ignored for costs with -nan
    if (cost < min_cost) {
      min_cost = cost;
      split.id = split_id;
      split.axis = axis;
      run_trough = false;
    }
  }
  if (run_trough) {
    split.axis = 3;  // Fallback to middle split
  }
  return split;
}
// -----------------------------------------------------------------------------
// Splitt node

void SAH::split_middle(bvh_node_pointer *node) {
  if (_tree->get_data(node)->triangle_ids.size() <= _max_triangles) {
    return;
  }

  split_middle_node(node);

  split_middle(_tree->get_left(node));
  split_middle(_tree->get_right(node));
}

void SAH::split_middle_node(bvh_node_pointer *node) {
  // asing child nodes
  BVH_node_data data_left;
  BVH_node_data data_right;

  // TODO(tobi) insert child withou BVH_node_data parameter
  bvh_node_pointer *node_left = _tree->insert_child(data_left, node);
  bvh_node_pointer *node_right = _tree->insert_child(data_right, node);

  uint axis = _tree->get_longest_axis(node);
  node->data.axis = axis;

  uint size = _tree->get_data(node)->triangle_ids.size();

  sort(_tree->get_data(node)->triangle_ids.begin(), size, axis);

  for (size_t i = 0; i < size; i++) {
    uint id = _tree->get_data(node)->triangle_ids[i];
    if (i < size / 2) {
      _tree->get_data(node_left)->triangle_ids.push_back(id);
    } else {
      _tree->get_data(node_right)->triangle_ids.push_back(id);
    }
  }

  // calculate bounding boxes
  _tree->calculate_min(node_left);
  _tree->calculate_max(node_left);

  _tree->calculate_min(node_right);
  _tree->calculate_max(node_right);

  _tree->free_triangles(node);
}

/**
 * @brief
 *
 * @param node_id
 * @param axis which to split
 * @param distance beetween split and min of bounding box in given axis
 */
void SAH::split(bvh_node_pointer *node, const SAH_buckets &buckets,
                const split_point &splitp) {
  // Split node at triangle with split id
  // update node
  if (splitp.axis >= 3) {
    split_middle_node(node);
    return;
  }

  node->data.axis = splitp.axis;
  // asing child nodes
  BVH_node_data data_left;
  BVH_node_data data_right;

#ifdef SPLIT_LONGEST_AXIS
  combine_ids(&data_left.triangle_ids, buckets, 0, 0, splitp.id);
  combine_ids(&data_right.triangle_ids, buckets, 0, splitp.id + 1,
              SAH_NUM_BUCKETS - 1);
  data_left.bounds = combine_box(buckets, 0, 0, splitp.id);
  data_right.bounds =
      combine_box(buckets, 0, splitp.id + 1, SAH_NUM_BUCKETS - 1);
#else
  combine_ids(&data_left.triangle_ids, buckets, splitp.axis, 0, splitp.id);
  combine_ids(&data_right.triangle_ids, buckets, splitp.axis, splitp.id + 1,
              SAH_NUM_BUCKETS - 1);
  data_left.bounds = combine_box(buckets, splitp.axis, 0, splitp.id);
  data_right.bounds =
      combine_box(buckets, splitp.axis, splitp.id + 1, SAH_NUM_BUCKETS - 1);
#endif

  // TODO(tobi) insert child withou BVH_node_data parameter
  bvh_node_pointer *node_left = _tree->insert_child(data_left, node);
  bvh_node_pointer *node_right = _tree->insert_child(data_right, node);
  // clear triangles from node to save memory
  _tree->free_triangles(node);
}

void SAH::split(bvh_node_pointer *node) {
  // check if split needed -> triangles > 2
  uint num_triangles = _tree->get_data(node)->triangle_ids.size();
  if (num_triangles <= _max_triangles) {
    return;
  }
  if (num_triangles <= MIN_SAH_SPLIT) {
    split_middle(node);
    return;
  }

  SAH_buckets buckets;
  // sort triangles into buckets
#ifndef SPLIT_LONGEST_AXIS
  triangles_into_buckets(node, &buckets);

  // calculate costs for every split
  split_point splitp = calc_min_split(node, &buckets);
#else
  uint axis = triangles_into_buckets_axis(node, &buckets);
  split_point splitp = calc_min_split(node, &buckets, axis);
#endif
  // std::cout << "best split: " << splitp.id << "\t axis:" << splitp.axis <<
  // "\n";

  if (splitp.axis < 4) {
    split(node, buckets, splitp);

    // recursivley continue splitting
    split(_tree->get_left(node));
    split(_tree->get_right(node));
  }
}

void SAH::built_on_treelets() {
  // define new root node
  BVH_node_data data{};

  uint num_treelets = _tree->get_treelets().size();
  for (uint i = 0; i < num_treelets; i++) {
    data.triangle_ids.push_back(i);
  }

  _tree->set_root(data);

  split_treelets(_tree->get_root());

  _tree->clear_treelets();
}
void SAH::split_middle_node_treelets(bvh_node_pointer *node) {
  uint count = node->data.triangle_ids.size();

  // asing child nodes
  BVH_node_data data_left;
  BVH_node_data data_right;

  // TODO(tobi) insert child withou BVH_node_data parameter
  bvh_node_pointer *node_left = _tree->insert_child(data_left, node);
  bvh_node_pointer *node_right = _tree->insert_child(data_right, node);

  uint axis = _tree->get_longest_axis(node);
  node->data.axis = axis;

  uint size = _tree->get_data(node)->triangle_ids.size();

  sort_treelets(_tree->get_data(node)->triangle_ids.begin(), size, axis);

  for (size_t i = 0; i < size; i++) {
    uint id = _tree->get_data(node)->triangle_ids[i];
    if (i < size / 2) {
      _tree->get_data(node_left)->triangle_ids.push_back(id);
    } else {
      _tree->get_data(node_right)->triangle_ids.push_back(id);
    }
  }
}

void SAH::split_treelets(bvh_node_pointer *node) {
  uint count = node->data.triangle_ids.size();
  // calculate bounding box
  node->data.bounds = combine_box_treelets(node);

  bvh_node_pointer *parent = node->parent;
  if (count == 1) {
    uint id = node->data.triangle_ids.at(0);
    // add treelet to tree
    if (parent->left == node) {
      parent->left = _tree->get_treelet(id);
      *(_tree->get_treelets().data() + id) = nullptr;
      delete node;
    } else if (parent->right == node) {
      parent->right = _tree->get_treelet(id);
      *(_tree->get_treelets().data() + id) = nullptr;
      delete node;
    } else {
      throw std::runtime_error("parent is not a child of this node!");
    }
    return;
  }
  if (count <= MIN_SAH_SPLIT) {
    split_middle_node_treelets(node);
    split_treelets(node->left);
    split_treelets(node->right);
    return;
  }

  SAH_buckets buckets;

#ifndef SPLIT_LONGEST_AXIS
  treelets_into_buckets(node, &buckets);

  split_point split = calc_min_split_treelets(node, &buckets);
#else
  uint axis = treelets_into_buckets_axis(node, &buckets);
  split_point split = calc_min_split_treelets(node, &buckets, axis);
#endif

  // fallback middle split
  if (split.axis == 3) {
    split_middle_node_treelets(node);
    split_treelets(node->left);
    split_treelets(node->right);
    return;
  }

  BVH_node_data data_left = BVH_node_data();
  BVH_node_data data_right = BVH_node_data();

  // TODO(tobi) combineids
#ifdef SPLIT_LONGEST_AXIS
  combine_ids(&data_left.triangle_ids, buckets, 0, 0, split.id);
  combine_ids(&data_right.triangle_ids, buckets, 0, split.id + 1,
              SAH_NUM_BUCKETS - 1);
#else
  combine_ids(&data_left.triangle_ids, buckets, split.axis, 0, split.id);
  combine_ids(&data_right.triangle_ids, buckets, split.axis, split.id + 1,
              SAH_NUM_BUCKETS - 1);
#endif

  bvh_node_pointer *left = _tree->insert_child(data_left, node);
  bvh_node_pointer *right = _tree->insert_child(data_right, node);

  _tree->free_triangles(node);

  split_treelets(left);
  split_treelets(right);
}
