#ifndef NODE_STRUCTURE_H
#define NODE_STRUCTURE_H

#include <bits/stdc++.h>
using namespace std;

extern int block_size;
extern int degree;
extern int key_len;
extern long root_address;
extern string index_filename;
extern string data_filename;

class Node {
    public:
    long address;
    bool is_leaf;
    vector<string> keys;
    vector<long> children;
    vector<long> pointers;
    long next;
    long prev;

    Node(long addr);
    Node(bool leaf, string key, long val, long ptr);
    Node(bool leaf, vector<string> keys_, vector<long> vals, vector<long> ptrs);

    void write_to_disk();
    void read_from_disk();
    void flush_node();
    Node* get_child(int idx);
};

#endif
