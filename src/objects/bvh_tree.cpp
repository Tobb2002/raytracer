#include "bvh_tree.hpp"
#include <stdexcept>

BVH_tree::BVH_tree(BVH_node_data root_data) {
  root = new bvh_node;
  root->data = root_data;
}

/// @brif copy_constructor to initialize new tree
BVH_tree::BVH_tree(const BVH_tree& old_tree) {
  triangles_flat = old_tree.triangles_flat;
  root = copy_node(old_tree.root);
}

BVH_tree& BVH_tree::operator=(const BVH_tree& old_tree) {
  triangles_flat = old_tree.triangles_flat;
  root = copy_node(old_tree.root);
  return *this;
}

bvh_node* BVH_tree::copy_node(bvh_node *old_node) {
  bvh_node *new_node = nullptr;
  if (old_node != nullptr) {
    new_node = new bvh_node;
    new_node->data = old_node->data;
    new_node->left = copy_node(old_node->left);
    new_node->right = copy_node(old_node->right);
  }
  return new_node;
}

BVH_tree::~BVH_tree() {
  destroy_tree();
}


bvh_node* BVH_tree::insert_child(BVH_node_data data, bvh_node *node) {
  if (node == nullptr) {
    throw std::runtime_error("insert child: Node does not exist!");
  }
  
  if (node->left == nullptr) {
    // no left child -> insert as left
    node->left = new bvh_node;
    node->left->parent = node;
    node->left->data = data;
    return node->left;
  }
  if (node->right == nullptr) {
    // no right child -> insert as left
    node->right = new bvh_node;
    node->right->parent = node;
    node->right->data = data;
    return node->right;
  }
  throw std::runtime_error("insert child: node allready has two childs!");

}

bvh_node* BVH_tree::get_root() {
  if (!root) {
    throw std::runtime_error("No root node!");
  }
  return root;
}

bvh_node* BVH_tree::get_left(bvh_node* node){
  if (node == nullptr || node->left == nullptr) {
    throw std::runtime_error("Node does not exist or has no left child!");
  }
  return node->left;
}

bvh_node* BVH_tree::get_right(bvh_node* node){
  if (node == nullptr || node->right == nullptr) {
    throw std::runtime_error("Node does not exist or has no right child!");
  }
  return node->right;
}

BVH_node_data* BVH_tree::get_data(bvh_node* node) {
  if (!node) {
    throw std::runtime_error("Node does not exist!");
  }
  return &node->data;
}

bool BVH_tree::is_leaf(bvh_node* node) {
  if (!node->left && !node->right) {
    return true;
  }
  return false;
}
void BVH_tree::free_triangles(bvh_node* node) {
  node->data.triangle_ids.clear();
}
// private

void BVH_tree::destroy_node(bvh_node *node) {
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
}
