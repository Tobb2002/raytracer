/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "bvh_tree.hpp"

#include <stdexcept>

BVH_tree::BVH_tree(BVH_node_data root_data, std::vector<Triangle>* triangles) {
  root = new bvh_node_pointer;
  root->data = root_data;
  _triangles = triangles;
}

/// @brif copy_constructor to initialize new tree
BVH_tree::BVH_tree(const BVH_tree& old_tree) {
  _triangles_flat = old_tree._triangles_flat;
  _triangles = old_tree._triangles;
  destroy_tree();
  root = copy_node(old_tree.root);
}

BVH_tree& BVH_tree::operator=(const BVH_tree& old_tree) {
  _triangles_flat = old_tree._triangles_flat;
  _triangles = old_tree._triangles;
  destroy_tree();
  root = copy_node(old_tree.root);
  // copy treelets
  for (bvh_node_pointer* treelet : old_tree._treelets) {
    _treelets.push_back(copy_node(treelet));
  }
  return *this;
}

bvh_node_pointer* BVH_tree::copy_node(bvh_node_pointer* old_node) {
  bvh_node_pointer* new_node = nullptr;
  if (old_node != nullptr) {
    new_node = new bvh_node_pointer;
    new_node->data = old_node->data;
    new_node->left = copy_node(old_node->left);
    new_node->right = copy_node(old_node->right);
  }
  return new_node;
}

BVH_tree::~BVH_tree() {
  destroy_tree();
  destroy_treelets();
}

void BVH_tree::set_triangles(std::vector<Triangle>* triangles) {
  _triangles = triangles;
}

std::vector<Triangle>* BVH_tree::get_triangle_vec() { return _triangles; }

bvh_node_pointer* BVH_tree::insert_child(BVH_node_data data,
                                         bvh_node_pointer* node) {
  if (node == nullptr) {
    throw std::runtime_error("insert child: Node does not exist!");
  }

  if (node->left == nullptr) {
    // no left child -> insert as left
    node->left = new bvh_node_pointer;
    node->left->parent = node;
    node->left->data = data;
    return node->left;
  }
  if (node->right == nullptr) {
    // no right child -> insert as left
    node->right = new bvh_node_pointer;
    node->right->parent = node;
    node->right->data = data;
    return node->right;
  }
  throw std::runtime_error("insert child: node allready has two childs!");
}

bvh_node_pointer* BVH_tree::get_root() {
  if (!root) {
    throw std::runtime_error("No root node!");
  }
  return root;
}
void BVH_tree::set_root(BVH_node_data data) {
  if (root != nullptr) {
    throw std::runtime_error("tree already has a root node!");
  }

  root = new bvh_node_pointer(data);
}

void BVH_tree::delete_root() { delete root; }

bvh_node_flat* BVH_tree::get_root_flat() { return _triangles_flat.data(); }

bvh_node_pointer* BVH_tree::get_left(bvh_node_pointer* node) {
  if (node == nullptr || node->left == nullptr) {
    throw std::runtime_error("Node does not exist or has no left child!");
  }
  return node->left;
}

bvh_node_pointer* BVH_tree::get_right(bvh_node_pointer* node) {
  if (node == nullptr || node->right == nullptr) {
    throw std::runtime_error("Node does not exist or has no right child!");
  }
  return node->right;
}

BVH_node_data* BVH_tree::get_data(bvh_node_pointer* node) {
  if (!node) {
    throw std::runtime_error("Node does not exist!");
  }
  return &node->data;
}

// ----------------------------------------------------------------------------
// Operations

uint BVH_tree::get_longest_axis(bvh_node_pointer* node) {
  float longest = 0;
  uint res = 0;
  for (int i = 0; i < 3; i++) {
    float length =
        get_data(node)->bounds.max[i] - get_data(node)->bounds.min[i];
    if (length > longest) {
      longest = length;
      res = i;
    }
  }
  return res;
}

bvh_box BVH_tree::update_box(bvh_node_pointer* node) {
  if (is_leaf(node)) {
    calculate_bounds(node);
    return {get_data(node)->bounds.min, get_data(node)->bounds.max};
  }
  bvh_box left = update_box(get_left(node));
  bvh_box right = update_box(get_right(node));
  update_min(&left.min, right.min);
  update_max(&left.max, right.max);

  get_data(node)->bounds.min = left.min;
  get_data(node)->bounds.max = left.max;

  return {get_data(node)->bounds.min, get_data(node)->bounds.max};
}

/**
 * @brief Goes throug all triangles in node and calculates min value;
 *
 * @param node_id
 * @return vec3
 */
void BVH_tree::calculate_min(bvh_node_pointer* node) {
  for (uint i : get_data(node)->triangle_ids) {
    Triangle* t = (_triangles->data() + i);
    update_min(&get_data(node)->bounds.min, t->get_min_bounding());
  }
}

void BVH_tree::calculate_max(bvh_node_pointer* node) {
  for (uint i : get_data(node)->triangle_ids) {
    Triangle* t = (_triangles->data() + i);
    update_max(&get_data(node)->bounds.max, t->get_max_bounding());
  }
}

void BVH_tree::calculate_bounds(bvh_node_pointer* node) {
  calculate_min(node);
  calculate_max(node);
}

void BVH_tree::update_min(vec3* min, const vec3& min_value) {
  for (int i = 0; i < 3; i++) {
    // update min
    if (min_value[i] < (*min)[i]) {
      (*min)[i] = min_value[i];
    }
  }
}

void BVH_tree::update_max(vec3* max, const vec3& max_value) {
  for (int i = 0; i < 3; i++) {
    // update min
    if (max_value[i] > (*max)[i]) {
      (*max)[i] = max_value[i];
    }
  }
}

void BVH_tree::update_bounds(vec3* min, const vec3& min_value, vec3* max,
                             const vec3& max_value) {
  update_min(min, min_value);
  update_max(max, max_value);
}

vec3 BVH_tree::get_middle(bvh_box box) {
  return box.min + (box.max - box.min) * 0.5f;
}

Triangle* BVH_tree::get_triangle(uint id) { return _triangles->data() + id; }

uint BVH_tree::get_left(uint id_flat) { return id_flat + 1; }
uint BVH_tree::get_right(uint id_flat) {
  return id_flat + get_node(id_flat)->offset_right;
}

bvh_node_flat* BVH_tree::get_node(uint id_flat) {
  return _triangles_flat.data() + id_flat;
}

BVH_node_data* BVH_tree::get_data(uint id_flat) {
  return &(_triangles_flat.data() + id_flat)->data;
}

bool BVH_tree::is_leaf(bvh_node_pointer* node) {
  if (!node->left && !node->right) {
    return true;
  }
  return false;
}
void BVH_tree::free_triangles(bvh_node_pointer* node) {
  node->data.triangle_ids.clear();
}

void BVH_tree::flatten_tree() {
  // traverse in depth first search order and append items to array.
  flatten_node(get_root());
  destroy_tree();
}

uint BVH_tree::flatten_node(bvh_node_pointer* node) {
  uint index = _triangles_flat.size();
  bvh_node_flat data = bvh_node_flat(node->data, 0, is_leaf(node));
  _triangles_flat.push_back(data);

  if (is_leaf(node)) {
    return index;
  }
  flatten_node(get_left(node));
  uint index_right = flatten_node(get_right(node));

  (_triangles_flat.data() + index)->offset_right = index_right - index;

  return index;
}
// private

void BVH_tree::destroy_node(bvh_node_pointer* node) {
  if (node == nullptr) {
    return;
  }
  destroy_node(node->left);
  destroy_node(node->right);

  // destroy this node
  delete node;
}
void BVH_tree::destroy_tree() {
  destroy_node(root);
  root = nullptr;
}

void BVH_tree::destroy_treelets() {
  for (bvh_node_pointer* node : _treelets) {
    destroy_node(node);
  }
}

// treelets
void BVH_tree::add_treelet(const std::vector<uint>& treelet_ids) {
  BVH_node_data data;
  for (uint id : treelet_ids) {
    data.triangle_ids.push_back(id);
  }

  bvh_node_pointer* node = new bvh_node_pointer(data);
  calculate_bounds(node);

  _treelets.push_back(node);
}

std::vector<bvh_node_pointer*> BVH_tree::get_treelets() { return _treelets; }
bvh_node_pointer* BVH_tree::get_treelet(uint id) { return _treelets.at(id); }
void BVH_tree::clear_treelets() { _treelets.clear(); }
