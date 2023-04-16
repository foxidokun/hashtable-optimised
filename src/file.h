#ifndef HASHMAP_OPTIMISED_FILE_H
#define HASHMAP_OPTIMISED_FILE_H

#include "hashmap.h"
#include "common.h"

namespace hashmap {
    result_t populate(hashmap_t *self, const char *filename);
}

#endif //HASHMAP_OPTIMISED_FILE_H
