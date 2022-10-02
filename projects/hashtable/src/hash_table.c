//
// Created by fokysland on 30.09.22.
//

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hash_table.h"

#define HT_INITIAL_SIZE 50
#define HT_PRIME_1 131
#define HT_PRIME_2 457

static ht_item HT_DELETED_ITEM = {NULL, NULL};

static ht_item *ht_new_item(const char *key, const char *value) {
    ht_item *item = malloc(sizeof(ht_item));
    item->key = strdup(key);
    item->value = strdup(value);
    return item;
}

static void ht_delete_item(ht_item *item) {
    free(item->key);
    free(item->value);
    free(item);
}

ht_hash_table *ht_new() {
    ht_hash_table *ht = malloc(sizeof(ht_hash_table));
    ht->size = HT_INITIAL_SIZE;
    ht->count = 0;
    ht->items = calloc(ht->size, sizeof(ht_item *));
    return ht;
}

void ht_delete_hash_table(ht_hash_table *ht) {
    for (size_t i = 0; i < ht->size; ++i) {
        ht_item *item = ht->items[i];
        if (item) {
            ht_delete_item(item);
        }
    }

    free(ht->items);
    free(ht);
}

static int ht_hash(const char *s, const int prime, const int max_value) {
    long hash = 0;
    const size_t len_s = strlen(s);
    for (size_t i = 0; i < len_s; i++) {
        hash += (long) pow(prime, (double) (len_s - (i + 1))) * s[i];
        hash %= max_value;
    }
    return (int) hash;
}

static int ht_get_hash(
        const char *s, const int num_buckets, const int attempt
) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % (int) num_buckets;
}

void ht_insert(ht_hash_table *ht, const char *key, const char *value) {
    ht_item *item = ht_new_item(key, value);
    int prev_size = (int) ht->size;

    int index = ht_get_hash(item->key, prev_size, 0);
    ht_item *cur_item = ht->items[index];
    int i = 1;
    while (cur_item) {
        if (cur_item != &HT_DELETED_ITEM && strcmp(cur_item->key, key) == 0) {
            ht_delete_item(cur_item);
            ht->items[index] = item;
            return;
        }

        index = ht_get_hash(item->key, prev_size, i);
        cur_item = ht->items[index];
        i++;
    }

    ht->items[index] = item;
    ht->count++;
}

char *ht_search(ht_hash_table *ht, const char *key) {
    int size = (int) ht->size;
    int index = ht_get_hash(key, size, 0);
    ht_item *item = ht->items[index];
    int i = 1;

    while (item) {
        if (item != &HT_DELETED_ITEM && strcmp(item->key, key) == 0) {
            return item->value;
        }
        index = ht_get_hash(key, size, i);
        item = ht->items[index];
        i++;
    }

    return NULL;
}

void ht_delete(ht_hash_table *ht, const char *key) {
    int size = (int) (ht->size);

    int index = ht_get_hash(key, size, 0);

    ht_item *item = ht->items[index];
    int i = 1;
    while (item) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                ht_delete_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }

        index = ht_get_hash(key, size, i);
        item = ht->items[index];
        i++;
    }

    ht->count--;
}


