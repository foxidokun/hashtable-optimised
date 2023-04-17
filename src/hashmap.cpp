#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "hashmap.h"
#include "prime_numbers.h"

// ---------------------------------------------------------------------------------------------------------------------

static double_node_t *node_new();

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
        if (current->value) free(current->value);
        current = current->next;

        while (current != nullptr) {
            next = current->next;

            free(current->value);
            free(current);

            current = next;
        }
    }

    free(self->buckets);
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

    while (bucket->value != nullptr) {
        if (bucket->next == nullptr) {
            bucket->next = node_new();
        }

        bucket = bucket->next;
    }

    strcpy(bucket->key, key);
    bucket->value = strdup(value);

    self->size++;
}

// ---------------------------------------------------------------------------------------------------------------------

char *hashmap::find(hashmap_t *self, const char *key) {
    assert(strlen(key) < 32);

    size_t hash = self->hash_func(key) % self->bucket_len;
    double_node_t *bucket = self->buckets + hash;

    while (true) {
        if (strcmp(key, bucket->key) == 0) {
            return bucket->value;
        }

        if (bucket->next) {
            bucket = bucket->next;
        } else {
            return nullptr;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

static double_node_t *node_new() {
    return (double_node_t *) calloc(1, sizeof (double_node_t));
}