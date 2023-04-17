#ifndef HASHMAP_OPTIMISED_BENCHMARK_H
#define HASHMAP_OPTIMISED_BENCHMARK_H

#include "common.h"
#include "hash.h"
#include "file.h"

result_t INTERNAL_gen_hash_stat(size_t size, hash_func_t hash_func, database_t *database,
                                const char *hash_func_name, const char *filename);

result_t print_hash_stats(hashmap_t *self, const char *hash_func_name, const char *filename);

result_t benchmark_search(hashmap_t *self, database_t *database, const char *report_filename);

#define gen_hash_stats(size, hash_func, dict) INTERNAL_gen_hash_stat(size, hash_func, dict, #hash_func, REPORTS_DIR #hash_func ".json")

#endif //HASHMAP_OPTIMISED_BENCHMARK_H
