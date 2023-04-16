#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "hashmap.h"
#include "prime_numbers.h"

// ---------------------------------------------------------------------------------------------------------------------

hashmap_t *hashmap::ctor(size_t requested_size, hash_func_t hash_func) {
    hashmap_t *self = (hashmap_t *) calloc(1, sizeof(hashmap_t));

    self->hash_func  = hash_func;

    for (int i = 0; i < NUM_PRIME_NUMBERS; ++i) {
        if (PRIME_NUMBERS[i] > requested_size) {
            self->bucket_len = PRIME_NUMBERS[i];
            break;
        }
    }

    if (!self->bucket_len) {
        fprintf (stderr, "Warning: no such big prime number in database, using requested_size...\n");
        self->bucket_len = requested_size;
    }

    self->buckets = (double_node_t *) calloc(self->bucket_len, sizeof(double_node_t));

    assert (self->size == 0);
    return self;
}

// ---------------------------------------------------------------------------------------------------------------------

void hashmap::dtor(hashmap_t *self) {
    double_node_t *next = nullptr;
    double_node_t *current = nullptr;

    // Free memory used by double_nodes, allocated with malloc (self->bucket[i] 's childs)
    for (size_t i = 0; i < self->bucket_len; ++i) {
        current = self->buckets + i;

        while (current->next != nullptr) {
            next = current->next;

            free(current->value1);
            free(current->value2);
            free(current);

            current = next;
        }
    }

    free(self);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t hashmap::size(hashmap_t *self) {
    return self->size;
}

// ---------------------------------------------------------------------------------------------------------------------

void hashmap::insert(hashmap_t *self, const char *key, const char *value) {
    assert(strlen(key) < 32);

    size_t hash = self->hash_func(key) % self->bucket_len;

    double_node_t *bucket = self->buckets + hash;

    while (bucket->value2 != nullptr) {
        bucket = bucket->next;
    }

    if (bucket->value1 == nullptr) {
        strcpy(bucket->key1, key);
        bucket->value1 = strdup(value);
    } else {
        strcpy(bucket->key2, key);
        bucket->value2 = strdup(value);
    }

    self->size++;
}

// ---------------------------------------------------------------------------------------------------------------------

char *hashmap::find(hashmap_t *self, const char *key) {
    assert(strlen(key) < 32);

    size_t hash = self->hash_func(key) % self->bucket_len;
    double_node_t *bucket = self->buckets + hash;

    while (true) {
        if (strcmp(key, bucket->key1) == 0) {
            return bucket->value1;
        } else if (bucket->value2 && strcmp(key, bucket->key2) == 0) {
            return bucket->value2;
        }

        if (bucket->next) {
            bucket = bucket->next;
        } else {
            return nullptr;
        }
    }
}