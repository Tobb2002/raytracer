/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "bvh_tree.hpp"

#include <stdexcept>

BVH_tree::BVH_tree(BVH_node_data root_data, std::vector<Triangle> *triangles) {
  root = new bvh_node_pointer;
  root->data = root_data;
  _triangles = triangles;
}

/// @brif copy_constructor to initialize new tree
BVH_tree::BVH_tree(const BVH_tree& old_tree) {
  triangles_flat = old_tree.triangles_flat;
  _triangles = old_tree._triangles;
  root = copy_node(old_tree.root);
}

BVH_tree& BVH_tree::operator=(const BVH_tree& old_tree) {
  triangles_flat = old_tree.triangles_flat;
  _triangles = old_tree._triangles;
  root = copy_node(old_tree.root);
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

BVH_tree::~BVH_tree() { destroy_tree(); }

void BVH_tree::set_triangles(std::vector<Triangle> *triangles) {
  _triangles = triangles;
}

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

bvh_node_flat* BVH_tree::get_root_flat() { return triangles_flat.data(); }

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

uint BVH_tree::get_longest_axis(bvh_node_pointer *node) {
  float longest = 0;
  uint res = 0;
  for (int i = 0; i < 3; i++) {
    float length = get_data(node)->bounds.max[i] -
                   get_data(node)->bounds.min[i];
    if (length > longest) {
      longest = length;
      res = i;
    }
  }
  return res;
}

bvh_box BVH_tree::update_box(bvh_node_pointer *node) {
  if (is_leaf(node)) {
    calculate_bounds(node);
    return {get_data(node)->bounds.min,
            get_data(node)->bounds.max};
  }
  bvh_box left = update_box(get_left(node));
  bvh_box right = update_box(get_left(node));
  update_min(&left.min, right.min);
  update_max(&left.max, right.max);

  get_data(node)->bounds.min = left.min;
  get_data(node)->bounds.max = left.max;

  return {get_data(node)->bounds.min,
          get_data(node)->bounds.max};
}

/**
 * @brief Goes throug all triangles in node and calculates min value;
 *
 * @param node_id
 * @return vec3
 */
void BVH_tree::calculate_min(bvh_node_pointer *node) {
  for (uint i : get_data(node)->triangle_ids) {
    Triangle *t = (_triangles->data() + i);
    update_min(&get_data(node)->bounds.min, t->get_min_bounding());
  }
}

void BVH_tree::calculate_max(bvh_node_pointer *node) {
  for (uint i : get_data(node)->triangle_ids) {
    Triangle *t = (_triangles->data() + i);
    update_max(&get_data(node)->bounds.max, t->get_max_bounding());
  }
}

void BVH_tree::calculate_bounds(bvh_node_pointer *node) {
  calculate_min(node);
  calculate_max(node);
}

void BVH_tree::update_min(vec3 *min, const vec3 &min_value) {
  for (int i = 0; i < 3; i++) {
    // update min
    if (min_value[i] < (*min)[i]) {
      (*min)[i] = min_value[i];
    }
  }
}

void BVH_tree::update_max(vec3 *max, const vec3 &max_value) {
  for (int i = 0; i < 3; i++) {
    // update min
    if (max_value[i] > (*max)[i]) {
      (*max)[i] = max_value[i];
    }
  }
}

void BVH_tree::update_bounds(vec3 *min, const vec3 &min_value, vec3 *max,
                        const vec3 &max_value) {
  update_min(min, min_value);
  update_max(max, max_value);
}

Triangle * BVH_tree::get_triangle(uint id) {
  return _triangles->data() + id;
}

bvh_node_flat* BVH_tree::get_left(bvh_node_flat* node) {}
bvh_node_flat* BVH_tree::get_right(bvh_node_flat* node) {}

bvh_node_flat* BVH_tree::get_data(bvh_node_flat* node) {}

bool BVH_tree::is_leaf(bvh_node_pointer* node) {
  if (!node->left && !node->right) {
    return true;
  }
  return false;
}
void BVH_tree::free_triangles(bvh_node_pointer* node) {
  node->data.triangle_ids.clear();
}

void BVH_tree::flatten() {}
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
void BVH_tree::destroy_tree() { destroy_node(root); }
