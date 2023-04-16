#include <stdio.h>
#include <chrono>
#include "common.h"
#include "hashmap.h"
#include "file.h"
#include "benchmark.h"

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

static void print_chain_lengths(hashmap_t *self, FILE *file);
static uint get_chain_length(double_node_t *node);
static long measure_one_search_iteration(hashmap_t *self, database_t *database);

// ---------------------------------------------------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------------------------------------------------

result_t INTERNAL_gen_hash_stat(size_t size, hash_func_t hash_func, database_t *database,
                                                                    const char *hash_func_name, const char *filename) {
    hashmap_t *map = hashmap::ctor(size, hash_func);
    hashmap::populate(map, database);

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

result_t benchmark_search(hashmap_t *self, database_t *database, const char *report_filename) {
    FILE *file = fopen(report_filename, "w");
    if (!file) { return result_t::ERROR; }

    long measures[REPEAT_NUM] = {};

    fprintf(file, R"({"elapsed_times": [)" "\n");

    for (int iter_num = 0; iter_num < REPEAT_NUM; ++iter_num) {
        measures[iter_num] = measure_one_search_iteration(self, database);

        if (iter_num+1 != REPEAT_NUM) {
            fprintf(file, "%ld, \n", measures[iter_num]);
        } else {
            fprintf(file, "%ld\n", measures[iter_num]);
        }
    }
    fprintf(file, "], \n");

    long long unsigned sum_time = 0;
    for (int i = 0; i < REPEAT_NUM; ++i) {
        sum_time += (unsigned long) measures[i];
    }

    fprintf(file, R"("avg_time": %llu)", sum_time / REPEAT_NUM);
    fprintf(file, "}\n");

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

static long measure_one_search_iteration(hashmap_t *self, database_t *database) {
    size_t key_count = database->len;
    char **keys = database->keys;

    auto begin = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < key_count; ++i) {
        hashmap::find(self, keys[i]);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    return elapsed.count();
}