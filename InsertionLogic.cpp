#include "InsertionLogic.h"
#include "NodeStructure.h"
#include <fstream>
using namespace std;

void create_index(string data_file, string index_file, int keylen, long new_root_address, bool update_flag);

Node* split_index_node(Node* index, string &parent_key) {
    parent_key = index->keys[degree];
    index->keys.erase(index->keys.begin() + degree);

    vector<string> new_keys;
    vector<long> new_children;

    new_children.push_back(index->children[degree + 1]);
    index->children.erase(index->children.begin() + degree + 1);

    while (index->keys.size() > degree) {
        new_keys.push_back(index->keys[degree]);
        index->keys.erase(index->keys.begin() + degree);
        new_children.push_back(index->children[degree + 1]);
        index->children.erase(index->children.begin() + degree + 1);
    }

    vector<long> v1;
    Node* new_node = new Node(false, new_keys, v1, new_children);

    return new_node;
}

Node* split_leaf_node(Node* leaf) {
    vector<string> new_keys;
    vector<long> new_pointers;

    for (int i = degree; i <= 2 * degree; i++) {
        new_keys.push_back(leaf->keys[i]);
        new_pointers.push_back(leaf->pointers[i]);
    }

    for (int i = degree; i <= 2 * degree; i++) {
        leaf->keys.pop_back();
        leaf->pointers.pop_back();
    }

    vector<long> v1;
    Node* new_node = new Node(true, new_keys, new_pointers, v1);
    return new_node;
}

Node* insert_record_in_btree(Node* root, string key, long offset) {
    root->read_from_disk();

    if (!root->is_leaf) {
        Node* index = root;
        int posn_key = 0;

        while (posn_key < index->keys.size()) {
            if (key.compare(index->keys[posn_key]) < 0)
                break;
            posn_key++;
        }

        Node* newchild = insert_record_in_btree(index->get_child(posn_key), key, offset);

        if (newchild == NULL)
            return NULL;

        int key_idx = 0;
        string newchild_key = newchild->keys[0];
        while (key_idx < index->keys.size()) {
            if (newchild_key.compare(index->keys[key_idx]) < 0)
                break;
            key_idx++;
        }

        if (key_idx >= index->keys.size()) {
            index->keys.push_back(newchild_key);
            index->children.push_back(newchild->address);
        } else {
            index->keys.insert(index->keys.begin() + key_idx, newchild_key);
            index->children.insert(index->children.begin() + key_idx + 1, newchild->address);
        }

        if (index->keys.size() <= 2 * degree) {
            index->write_to_disk();
            return NULL;
        } else {
            string parent_key = "";
            newchild = split_index_node(index, parent_key);

            if (index->address == root_address) {
                index->write_to_disk();
                newchild->write_to_disk();

                vector<long> new_children;
                new_children.push_back(index->address);
                new_children.push_back(newchild->address);
                vector<string> newkeys;
                newkeys.push_back(parent_key);
                vector<long> v1;

                Node* new_root = new Node(false, newkeys, v1, new_children);
                new_root->write_to_disk();
                root_address = new_root->address;
                update_metadata();
                return NULL;
            }
            return newchild;
        }
    } else {
        Node* leaf = root;

        if (key.compare(leaf->keys[0]) < 0) {
            leaf->keys.insert(leaf->keys.begin(), key);
            leaf->pointers.insert(leaf->pointers.begin(), offset);
        } else if (key.compare(leaf->keys[leaf->keys.size() - 1]) > 0) {
            leaf->keys.push_back(key);
            leaf->pointers.push_back(offset);
        } else {
            for (int key_idx = 0; key_idx < leaf->keys.size(); key_idx++) {
                if (leaf->keys[key_idx].compare(key) > 0) {
                    leaf->keys.insert(leaf->keys.begin() + key_idx, key);
                    leaf->pointers.insert(leaf->pointers.begin() + key_idx, offset);
                    break;
                }
            }
        }

        if (leaf->keys.size() <= 2 * degree) {
            leaf->write_to_disk();
            return NULL;
        } else {
            Node* newchild = split_leaf_node(leaf);

            if (leaf->address == root_address) {
                vector<string> newkeys;
                newkeys.push_back(newchild->keys[0]);

                long tmp = leaf->next;
                newchild->prev = leaf->address;
                newchild->next = tmp;

                if (tmp != -1) {
                    Node *n = new Node(tmp);
                    n->prev = newchild->address;
                    n->write_to_disk();
                }

                newchild->write_to_disk();
                leaf->next = newchild->address;
                leaf->write_to_disk();

                vector<long> new_children;
                new_children.push_back(leaf->address);
                new_children.push_back(newchild->address);
                vector<string> newkeys_;
                newkeys_.push_back(newchild->keys[0]);
                vector<long> v1;

                Node* new_root = new Node(false, newkeys_, v1, new_children);
                new_root->is_leaf = false;
                new_root->write_to_disk();
                root_address = new_root->address;
                update_metadata();
                return NULL;
            } else {
                newchild->write_to_disk();
                newchild->read_from_disk();
                long tmp = leaf->next;
                leaf->next = newchild->address;
                newchild->prev = leaf->address;
                newchild->next = tmp;

                if (tmp != -1) {
                    Node *n = new Node(tmp);
                    n->prev = newchild->address;
                    n->write_to_disk();
                }

                newchild->write_to_disk();
                leaf->write_to_disk();
            }
            return newchild;
        }
    }
}

// Update root metadata in the file
void update_metadata() {
    create_index(data_filename, index_filename, key_len, root_address, true);
}
