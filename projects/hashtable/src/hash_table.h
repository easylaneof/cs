//
// Created by fokysland on 30.09.22.
//

#ifndef HASHTABLE_HASH_TABLE_H

typedef struct {
    char *key;
    char *value;
} ht_item;

typedef struct {
    size_t size;
    size_t count;
    ht_item **items;
} ht_hash_table;


#define HASHTABLE_HASH_TABLE_H

#endif //HASHTABLE_HASH_TABLE_H
