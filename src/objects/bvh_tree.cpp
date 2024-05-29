#include "bvh_tree.hpp"
#include <stdexcept>

BVH_tree::BVH_tree(BVH_node_data root_data) {
  root = new bvh_node;
  root->data = root_data;
}

BVH_tree::~BVH_tree() {
  destroy_tree();
}


void BVH_tree::insert_child(BVH_node_data data, bvh_node *node) {
  if (node == nullptr) {
    throw std::runtime_error("Node does not exist or has no right child!");
  }

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
  if (node == nullptr) {
    throw std::runtime_error("Node does not exist or has no right child!");
  }
  return &node->data;
}

bool BVH_tree::is_leaf(bvh_node* node) {
  if (node->left == nullptr && node->right == nullptr) {
    return true;
  }
  return false;
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
