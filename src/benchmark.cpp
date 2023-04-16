#include <stdio.h>
#include "common.h"
#include "hashmap.h"
#include "file.h"
#include "benchmark.h"

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

static void print_chain_lengths(hashmap_t *self, FILE *file);
static uint get_chain_length(double_node_t *node);

// ---------------------------------------------------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------------------------------------------------

result_t INTERNAL_gen_hash_stat(size_t size, hash_func_t hash_func, const char *dictionary_file,
                                                                    const char *hash_func_name, const char *filename) {
    hashmap_t *map = hashmap::ctor(size, hash_func);
    result_t res = hashmap::populate(map, dictionary_file);
    UNWRAP_ERROR(res);

    print_hash_stats(map, hash_func_name, filename);

    hashmap::dtor(map);
    return result_t::OK;
}

// ---------------------------------------------------------------------------------------------------------------------

result_t print_hash_stats(hashmap_t *self, const char *hash_func_name, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) { return result_t::ERROR; }

    fprintf(file, "{\n");
    fprintf(file, R"("hash function": "%s",)" "\n", hash_func_name);
    fprintf(file, R"("size": %lu,)" "\n", self->size);
    fprintf(file, R"("n buckets": %lu,)" "\n", self->bucket_len);
    fprintf(file, R"("chain lengths": [)" "\n");

    print_chain_lengths(self, file);

    fprintf(file, "]\n}\n");
    fclose(file);

    return result_t::OK;
}

// ---------------------------------------------------------------------------------------------------------------------
// Static
// ---------------------------------------------------------------------------------------------------------------------

const int NUMBERS_IN_ROW = 30;

static void print_chain_lengths(hashmap_t *self, FILE *file) {
    for (size_t i = 0; i < self->bucket_len-1; ++i) {
        uint length = get_chain_length(self->buckets + i);

        fprintf(file, "%u, ", length);
        if ((i+1)%NUMBERS_IN_ROW == 0) {
            fprintf(file, "\n");
        }
    }

    uint length = get_chain_length(self->buckets + self->bucket_len - 1);

    fprintf(file, "%u", length);
}


static uint get_chain_length(double_node_t *node) {
    uint length = 0;

    while (node != nullptr) {
        if (node->value2) {
            length += 2;
        } else if (node->value1) {
            length++;
        }

        node = node->next;
    }

    return length;
}