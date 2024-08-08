/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */
#include "lbvh.hpp"

#include <boost/lambda/bind.hpp>
#include <cstdint>
#include <glm/gtx/string_cast.hpp>

#include "bvh_tree.hpp"

LBVH::LBVH(BVH_tree *tree) {
  _tree = tree;
  _morton = Morton(tree->get_triangle_vec(), GRID_SIZE);
}

void LBVH::split(bvh_node_pointer *node, uint split_id) {
  // asing child nodes
  BVH_node_data data_left;
  BVH_node_data data_right;

  bvh_node_pointer *node_left = _tree->insert_child(data_left, node);
  bvh_node_pointer *node_right = _tree->insert_child(data_right, node);

  // get split axis
  // node->data.axis = glm::mod(static_cast<float>(split_id), 3.f);

  // push back triangles
  BVH_node_data *data = _tree->get_data(node);
  size_t size = _tree->get_data(node)->triangle_ids.size();
  for (size_t i = 0; i < size; i++) {
    if (i < split_id) {
      node_left->data.triangle_ids.push_back(data->triangle_ids.at(i));
    } else {
      node_right->data.triangle_ids.push_back(data->triangle_ids.at(i));
    }
  }

  // calculate bounding boxes
  _tree->calculate_min(node_left);
  _tree->calculate_max(node_left);

  _tree->calculate_min(node_right);
  _tree->calculate_max(node_right);

  _tree->free_triangles(node);
}

void LBVH::split_first_bit(bvh_node_pointer *node, uint current_bit) {
  BVH_node_data *data = _tree->get_data(node);

  uint first_id = _tree->get_data(node)->triangle_ids.at(0);
  size_t size = _tree->get_data(node)->triangle_ids.size();

  if (size <= MAX_TRIANGLES) {
    return;
  }
  for (size_t i = current_bit; i > 0; i--) {
    bool first_bit = _morton.get_code(first_id) & (1 << i);
    for (size_t a = 0; a < size; a++) {
      uint id = _tree->get_data(node)->triangle_ids.at(a);
      bool sig_bit = _morton.get_code(id) & (1 << i);
      // iterate until most significant bit is different
      if (sig_bit != first_bit) {
        node->data.axis = glm::mod(static_cast<float>(i), 3.f);
        split(node, a);

        split_first_bit(_tree->get_left(node), i - 1);
        split_first_bit(_tree->get_right(node), i - 1);
        return;
      }
    }
  }
  // no split all childs should stay in the same child node
}

void LBVH::add_treelets(bvh_node_pointer *node) {
  BVH_node_data *data = _tree->get_data(node);

  uint64_t current_top_bits = 0;
  bool new_treelet = true;

  // triangle ids for current_treelet
  std::vector<uint> current_treelet;
  for (uint id : data->triangle_ids) {
    uint64_t morton_code = _morton.get_code(id);
    uint64_t top_bits =
        (morton_code >> (_morton.get_morton_size() - TREELET_BITS));

    if (new_treelet) {
      current_top_bits = top_bits;
      new_treelet = false;
    }

    if (current_top_bits == top_bits) {
      current_treelet.push_back(id);
    } else {
      // insert treelet
      _tree->add_treelet(current_treelet);
      current_treelet.clear();
      // initialize new treelet
      current_treelet.push_back(id);
      current_top_bits = top_bits;
    }
  }

  // at the rest to last treelet
  if (current_treelet.size() > 0) {
    _tree->add_treelet(current_treelet);
  }
}

void LBVH::build_treelets() {
  BVH_node_data *data_root = _tree->get_data(_tree->get_root());
  _morton.build(&data_root->triangle_ids, data_root->bounds);

  add_treelets(_tree->get_root());
  std::cout << "number of treelets: " << _tree->get_treelets().size();

  for (bvh_node_pointer *treelet : _tree->get_treelets()) {
    split_first_bit(treelet, _morton.get_morton_size() - TREELET_BITS);
  }

  _tree->destroy_tree();
}

void LBVH::build() {
  BVH_node_data *data_root = _tree->get_data(_tree->get_root());
  _morton.build(&data_root->triangle_ids, data_root->bounds);
  split_first_bit(_tree->get_root(),
                  _morton.get_morton_size());  // highest bit of morton code
}
