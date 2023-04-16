#ifndef HASHMAP_OPTIMISED_HASH_H
#define HASHMAP_OPTIMISED_HASH_H

#include <stdint.h>
#include <stdlib.h>

typedef uint64_t (*hash_func_t)(const char *obj);

uint64_t gnu_hash (const char *obj);

#endif //HASHMAP_OPTIMISED_HASH_H
