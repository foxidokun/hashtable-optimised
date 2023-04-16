#ifndef HASHMAP_OPTIMISED_BENCHMARK_H
#define HASHMAP_OPTIMISED_BENCHMARK_H

#include "common.h"
result_t INTERNAL_gen_hash_stat(size_t size, hash_func_t hash_func, const char *dictionary_file,
                                const char *hash_func_name, const char *filename);
result_t print_hash_stats(hashmap_t *self, const char *hash_func_name, const char *filename);

#define gen_hash_stats(size, hash_func, dict) INTERNAL_gen_hash_stat(size, hash_func, dict, #hash_func, REPORTS_DIR #hash_func ".json")

#endif //HASHMAP_OPTIMISED_BENCHMARK_H
