#ifndef MAIN_LOGIC_H
#define MAIN_LOGIC_H

#include<string>

void initialize_bplus_tree();
void create_index(std::string data_file, std::string index_file, int keylen, long new_root_address, bool update_flag = false);
void insert_record(std::string index_file, std::string initial_key);
void list_records(std::string index_file, std::string target_key, int count);
void find_index(std::string index_file, std::string target_key);
void update_metadata();

#endif
