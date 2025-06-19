#include "SearchList.h"
#include "NodeStructure.h"
#include <fstream>
#include <iostream>
using namespace std;

long find_record(Node* root, string key) {
    if (root == NULL)
        return -1;

    root->read_from_disk();
    if (root->is_leaf) {
        for (int key_idx = 0; key_idx < root->keys.size(); key_idx++) {
            if (key.compare(root->keys[key_idx]) == 0) {
                long p = root->pointers[key_idx];
                root->flush_node();
                return p;
            }
        }
        return -1;
    } else {
        for (int key_idx = 0; key_idx < root->keys.size(); key_idx++) {
            if (key.compare(root->keys[key_idx]) < 0) {
                Node* c = root->get_child(key_idx);
                root->flush_node();
                return find_record(c, key);
            }
        }
        Node* c = root->get_child(root->children.size() - 1);
        root->flush_node();
        return find_record(c, key);
    }
}

void print_record_at_offset(long key_offset) {
    char buf[1001] = "";
    ifstream infile(data_filename);
    infile.seekg(key_offset, infile.beg);
    infile.read(buf, 1000);
    infile.close();
    string str(buf);
    cout << str.substr(0, str.find("\n")) << endl;
}

void list_records_count(Node* root, string target_key, int count) {
    root->read_from_disk();
    if (root->is_leaf) {
        for (int i = 0; i < root->keys.size(); i++) {
            if (target_key.compare(root->keys[i]) == 0 ||
                (i > 0 && target_key.compare(root->keys[i - 1]) < 0 && target_key.compare(root->keys[i]) > 0) ||
                (root->keys.size() == 1)) {

                while (root && count > 0) {
                    for (int a = i; a < root->keys.size() && count > 0; a++) {
                        count--;
                        print_record_at_offset(root->pointers[a]);
                    }

                    if (root->next == -1) break;
                    long next_root = root->next;
                    root->flush_node();
                    root = new Node(next_root);
                }
                return;
            }
        }
    } else {
        for (int key_idx = 0; key_idx < root->keys.size(); key_idx++) {
            if (target_key.compare(root->keys[key_idx]) < 0) {
                Node* c = root->get_child(key_idx);
                root->flush_node();
                return list_records_count(c, target_key, count);
            }
        }
        Node* c = root->get_child(root->children.size() - 1);
        root->flush_node();
        return list_records_count(c, target_key, count);
    }
}
