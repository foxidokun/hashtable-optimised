#ifndef HASHMAP_OPTIMISED_FILE_H
#define HASHMAP_OPTIMISED_FILE_H

#include "hashmap.h"
#include "common.h"

struct mmaped_file_t {
    unsigned char *data;
    size_t size;
};

struct database_t {
    char (*keys)[KEY_SIZE];
    char **values;
    size_t len;

    // Чтобы освободить память
    mmaped_file_t file;
};

namespace hashmap {
    void populate(hashmap_t *self, database_t *database);
}

namespace database {
    database_t *load(const char *dictionary_file);
    void unload(database_t *self);
}

#endif //HASHMAP_OPTIMISED_FILE_H
