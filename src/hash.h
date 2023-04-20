#ifndef HASHMAP_OPTIMISED_HASH_H
#define HASHMAP_OPTIMISED_HASH_H

#include <stdint.h>
#include <stdlib.h>

typedef uint64_t (*hash_func_t)(const char *obj);

uint64_t gnu_hash (const char *obj);
uint64_t crc32_hash(const char *obj);
extern "C" uint64_t crc32_intrin_hash(const char *obj);
uint64_t rol_hash(const char *obj);
uint64_t ror_hash(const char *obj);
uint64_t const_hash(const char *);
uint64_t first_char_hash(const char *obj);
uint64_t sum_hash(const char *obj);
uint64_t strlen_hash(const char *obj);

#endif //HASHMAP_OPTIMISED_HASH_H
