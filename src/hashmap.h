#ifndef HASHMAP_OPTIMISED_HASHMAP_H
#define HASHMAP_OPTIMISED_HASHMAP_H

#include "hash.h"

const int KEY_SIZE = 32;

struct double_node_t {
    // 64 bytes for max cache utilisation
    char key1[KEY_SIZE]; //32
    char key2[KEY_SIZE]; //32

    // Other metadata in other cache line
    char *value1;        //8
    char *value2;        //8
    double_node_t *next; //8
};

struct hashmap_t {
    double_node_t *buckets;
    size_t bucket_len;

    size_t size;
    hash_func_t hash_func;
};

namespace hashmap {
    hashmap_t *ctor(size_t requested_size, hash_func_t hash_func);
    void dtor(hashmap_t *self);

    size_t size(hashmap_t *self);

    void insert(hashmap_t *self, const char key[KEY_SIZE], const char *value);
    char *find(hashmap_t *self, const char key[KEY_SIZE]);
}

#endif //HASHMAP_OPTIMISED_HASHMAP_H
