#include "NodeStructure.h"
#include "InsertionLogic.h"
#include "SearchList.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

// Function to initialize metadata from index file
void initialize_bplus_tree() {
    long offset = 0;
    char buffer[block_size];

    ifstream infile(index_filename, ios::in | ios::binary);
    infile.read(buffer, block_size);
    infile.close();

    string get_data_filename(buffer, 257);
    int end_idx = get_data_filename.find("0000"); // assumes no 4 zeroes in filename
    data_filename = get_data_filename.substr(0, end_idx);
    offset += 257;

    memcpy(&key_len, buffer + offset, sizeof(key_len));
    offset += sizeof(key_len);

    memcpy(&degree, buffer + offset, sizeof(degree));
    offset += sizeof(degree);

    memcpy(&root_address, buffer + offset, sizeof(root_address));
    offset += sizeof(root_address);
}

// Function to create a new index from data file
void create_index(string data_file, string index_file, int keylen, long new_root_address, bool update_flag = false) {
    long offset = 0;
    char buffer[block_size];

    string filename = data_file.append(string((256 - data_file.length()), '0'));
    memcpy(buffer + offset, filename.c_str(), strlen(filename.c_str()) + 1);
    offset += strlen(filename.c_str()) + 1;

    memcpy(buffer + offset, &keylen, sizeof(keylen));
    offset += sizeof(keylen);

    int degree = (block_size - 50) / ((keylen + 8) * 2);
    memcpy(buffer + offset, &degree, sizeof(degree));
    offset += sizeof(degree);

    if (new_root_address == -1) {
        new_root_address = 1024;
        root_address = 1024;
    }
    memcpy(buffer + offset, &new_root_address, sizeof(new_root_address));
    offset += sizeof(new_root_address);

    ofstream outfile;
    if (update_flag)
        outfile.open(index_file, ios::in | ios::out | ios::binary);
    else
        outfile.open(index_file, ios::out | ios::binary);
    outfile.write(buffer, block_size);
    outfile.close();

    if (update_flag) return;

    index_filename = index_file;
    initialize_bplus_tree();

    ifstream infile(data_filename);
    string line;
    offset = 0;
    int count = 0;
    Node* root;
    bool first_time = true;

    while (getline(infile, line)) {
        string key = line.substr(0, key_len);
        if (first_time) {
            root = new Node(true, key, offset, 0);
            first_time = false;
        } else {
            root = new Node(root_address);
        }

        insert_record_in_btree(root, key, offset);
        offset = infile.tellg();
        count++;
    }

    cout << "Successfully inserted " << count << " records in index file B++ Tree.\n";
}

// Find record by key and print
void find_index(string index_file, string target_key) {
    index_filename = index_file;
    initialize_bplus_tree();

    Node* root = new Node(root_address);

    if (target_key.length() > key_len)
        target_key = target_key.substr(0, key_len);
    else while (target_key.length() < key_len)
        target_key += " ";

    long key_offset = find_record(root, target_key);
    if (key_offset == -1)
        cout << "Cannot find specified record in index.\n";
    else
        print_record_at_offset(key_offset);
}

// Insert a new record to the data + index
void insert_record(string index_file, string initial_key) {
    index_filename = index_file;
    initialize_bplus_tree();

    if (key_len > initial_key.length()) {
        cout << "Input Error: key supplied is too short\n";
        return;
    }

    string key = initial_key.substr(0, key_len);
    Node* root = new Node(root_address);

    if (find_record(root, key) != -1) {
        cout << "Key already exists in the index.\n";
        return;
    }

    ofstream outfile(data_filename, ios::out | ios::binary | ios::app);
    long key_offset = outfile.tellp();
    initial_key = "\n" + initial_key;

    // cout << "Inserting \"" << initial_key << "\" at line number: " << key_offset << endl;

    outfile.write(initial_key.c_str(), initial_key.length());
    outfile.close();

    insert_record_in_btree(root, key, key_offset + 1); // +1 for newline
}

// List multiple records starting from a key
void list_records(string index_file, string target_key, int count) {
    index_filename = index_file;
    initialize_bplus_tree();
    Node* root = new Node(root_address);
    list_records_count(root, target_key, count);
}

// // Update root metadata
// void update_metadata() {
//     create_index(data_filename, index_filename, key_len, root_address, true);
// }

// Main driver
int main(int argc, char **argv) {
    if (argc < 4 || argc > 6) {
        cout << "Incorrect number of arguments\n";
        return 0;
    }

    string choice(argv[1]);

    if (choice == "-create") {
        string data_filename(argv[2]);
        if (data_filename.length() > 256) {
            cout << "Data file name too long. Please use < 256 characters.\n";
            return 0;
        }

        string index_file(argv[3]);
        int keylen = stoi(argv[4]);
        create_index(data_filename, index_file, keylen, -1, false);
    } else if (choice == "-find") {
        string index_file(argv[2]);
        string target_key(argv[3]);
        find_index(index_file, target_key);
    } else if (choice == "-insert") {
        string index_file(argv[2]);
        string target_key(argv[3]);
        insert_record(index_file, target_key);
    } else if (choice == "-list") {
        string index_file(argv[2]);
        string target_key(argv[3]);
        int count = stoi(argv[4]);
        list_records(index_file, target_key, count);
    }

    return 0;
}
