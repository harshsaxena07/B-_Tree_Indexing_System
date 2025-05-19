#ifndef SEARCH_AND_LIST_H
#define SEARCH_AND_LIST_H

#include "NodeStructure.h"

long find_record(Node* root, string key);
void print_record_at_offset(long key_offset);
void list_records_count(Node* root, string target_key, int count);

#endif
