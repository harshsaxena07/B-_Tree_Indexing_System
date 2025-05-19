#include "NodeStructure.h"
#include <fstream>
#include <cstring>
using namespace std;

// Define global variables (declared extern in the header)
int block_size = 1024;
int degree;
int key_len;
long root_address;
string index_filename;
string data_filename;

// Constructor: Load node from disk
Node::Node(long addr) {
    address = addr;
    read_from_disk();
}

// Constructor: Single key node
Node::Node(bool leaf, string key, long val, long ptr) {
    is_leaf = leaf;
    next = -1;
    prev = -1;
    address = -1;
    keys.push_back(key);

    if (leaf)
        pointers.push_back(val);
    else
        children.push_back(ptr);
}

// Constructor: Multi-key node
Node::Node(bool leaf, vector<string> keys_, vector<long> vals, vector<long> ptrs) {
    is_leaf = leaf;
    next = -1;
    prev = -1;
    address = -1;
    keys = keys_;

    if (leaf)
        pointers = vals;
    else
        children = ptrs;
}

// Write node to disk
void Node::write_to_disk() {
    if (address == -1) {
        ifstream infile(index_filename, ios::in | ios::binary);
        infile.seekg(0, ios::end);
        address = infile.tellg();
        infile.close();
    }

    long offset = 0;
    char buffer[1024];

    memcpy(buffer + offset, &is_leaf, sizeof(is_leaf));
    offset += sizeof(is_leaf);
    memcpy(buffer + offset, &next, sizeof(next));
    offset += sizeof(next);
    memcpy(buffer + offset, &prev, sizeof(prev));
    offset += sizeof(prev);

    long keys_size = keys.size();
    memcpy(buffer + offset, &keys_size, sizeof(keys_size));
    offset += sizeof(keys_size);

    for (string key : keys) {
        memcpy(buffer + offset, key.c_str(), strlen(key.c_str()) + 1);
        offset += strlen(key.c_str()) + 1;
    }

    if (!is_leaf) {
        for (long child : children) {
            memcpy(buffer + offset, &child, sizeof(child));
            offset += sizeof(child);
        }
    } else {
        for (long ptr : pointers) {
            memcpy(buffer + offset, &ptr, sizeof(ptr));
            offset += sizeof(ptr);
        }
    }

    ofstream outfile(index_filename, ios::out | ios::binary | ios::in);
    outfile.seekp(address, ios::beg);
    outfile.write(buffer, block_size);
    outfile.close();

    flush_node();
}

// Clear node from memory
void Node::flush_node() {
    is_leaf = false;
    next = -1;
    prev = -1;
    keys.clear();
    children.clear();
    pointers.clear();
}

// Read node from disk
void Node::read_from_disk() {
    if (address <= 0)
        return;

    char *buf = new char[block_size];
    long offset = 0;

    ifstream infile(index_filename, ios::in | ios::binary);
    infile.seekg(address);
    infile.read(buf, block_size);
    infile.close();

    memcpy(&is_leaf, buf + offset, sizeof(bool));
    offset += sizeof(is_leaf);
    memcpy(&next, buf + offset, sizeof(long));
    offset += sizeof(long);
    memcpy(&prev, buf + offset, sizeof(long));
    offset += sizeof(long);

    long keys_size;
    memcpy(&keys_size, buf + offset, sizeof(keys_size));
    offset += sizeof(keys_size);

    keys.clear();
    for (int i = 0; i < keys_size; i++) {
        string key(buf + offset, key_len);
        offset += key_len + 1;
        keys.push_back(key);
    }

    if (!is_leaf) {
        children.clear();
        for (int i = 0; i < keys_size + 1; i++) {
            long child;
            memcpy(&child, buf + offset, sizeof(child));
            offset += sizeof(child);
            children.push_back(child);
        }
    } else {
        pointers.clear();
        for (int i = 0; i < keys_size; i++) {
            long pointer;
            memcpy(&pointer, buf + offset, sizeof(pointer));
            offset += sizeof(pointer);
            pointers.push_back(pointer);
        }
    }
}

// Get child at index
Node* Node::get_child(int idx) {
    long addr = children[idx];
    Node *n = new Node(addr);
    return n;
}
