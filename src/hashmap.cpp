#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "hashmap.h"
#include "prime_numbers.h"

// ---------------------------------------------------------------------------------------------------------------------

static double_node_t *node_new();

static inline int asm_strcmp_inline(const char str1[KEY_SIZE], const char str2[KEY_SIZE]);
extern "C" __attribute__ ((noinline)) int asm_strcmp_noinline(const char str1[KEY_SIZE], const char str2[KEY_SIZE]);

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
        if (current->value1) free(current->value1);
        if (current->value2) free(current->value2);
        current = current->next;

        while (current != nullptr) {
            next = current->next;

            free(current->value1);
            free(current->value2);
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

void hashmap::insert(hashmap_t *self, const char key[KEY_SIZE], const char *value) {
    size_t hash = self->hash_func(key) % self->bucket_len;

    double_node_t *bucket = self->buckets + hash;

    while (bucket->value2 != nullptr) {
        if (bucket->next == nullptr) {
            bucket->next = node_new();
        }

        bucket = bucket->next;
    }

    if (bucket->value1 == nullptr) {
        strncpy(bucket->key1, key, KEY_SIZE);
        bucket->value1 = strdup(value);
    } else {
        strncpy(bucket->key2, key, KEY_SIZE);
        bucket->value2 = strdup(value);
    }

    self->size++;
}

// ---------------------------------------------------------------------------------------------------------------------

char * __attribute__ ((noinline)) hashmap::find(hashmap_t *self, const char key[KEY_SIZE]) {
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

// ---------------------------------------------------------------------------------------------------------------------

static double_node_t *node_new() {
    return (double_node_t *) calloc(1, sizeof (double_node_t));
}

// ---------------------------------------------------------------------------------------------------------------------

static  __attribute__ ((always_inline)) int asm_strcmp_inline(const char str1[KEY_SIZE], const char str2[KEY_SIZE]) {
    int res;

    asm inline (".intel_syntax noprefix\n"
        "        vmovdqa ymm0, YMMWORD PTR [%1]\n"  // Load aligned str1
        "        xor     %0, %0\n"                // Zero return value
        "\n"
        "        vptest  ymm0, YMMWORD PTR [%2]\n"  // test two strings
        "        seta    %b0\n"                     // set return value to planned
        "\n"
        "        vzeroupper\n"                      // https://www.agner.org/optimize/calling_conventions.pdf page 14
        ".att_syntax prefix\n"
        :  "=&r" (res) : "r" (str1), "r" (str2) : "ymm0", "cc");

    return res;
}