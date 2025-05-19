#ifndef INSERTION_LOGIC_H
#define INSERTION_LOGIC_H

#include "NodeStructure.h"

Node* insert_record_in_btree(Node* root, string key, long offset);
Node* split_index_node(Node* index, string &parent_key);
Node* split_leaf_node(Node* leaf);
void update_metadata();

#endif
