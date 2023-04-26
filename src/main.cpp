#include <stdio.h>

#include "common.h"
#include "hashmap.h"
#include "file.h"
#include "benchmark.h"

// ---------------------------------------------------------------------------------------------------------------------

const int HASHMAP_SIZE = 7583;
const char DICTIONARY_FILE[] = "data/dictbig.txt";

// ---------------------------------------------------------------------------------------------------------------------

void generate_statistics(database_t *database);
void benchmarking_search(database_t *database);
void benchmarking_hash(database_t *database);

// ---------------------------------------------------------------------------------------------------------------------

int main() {
    database_t *database = database::load(DICTIONARY_FILE);
    if (!database) {
        fprintf(stderr, "Failed to open dictionary file, see logs");
    }

//    generate_statistics(database);
//    printf("stats generated\n");
    benchmarking_search(database);
    printf("benchmarked\n");

//    benchmarking_hash(database);
//    printf("benchmarked hash\n");

    database::unload(database);
}

// ---------------------------------------------------------------------------------------------------------------------

void generate_statistics(database_t *database) {
        gen_hash_stats(HASHMAP_SIZE, gnu_hash,        database);
        gen_hash_stats(HASHMAP_SIZE, crc32_hash,      database);
        gen_hash_stats(HASHMAP_SIZE, rol_hash,        database);
        gen_hash_stats(HASHMAP_SIZE, ror_hash,        database);
        gen_hash_stats(HASHMAP_SIZE, const_hash,      database);
        gen_hash_stats(HASHMAP_SIZE, first_char_hash, database);
        gen_hash_stats(HASHMAP_SIZE, sum_hash,        database);
        gen_hash_stats(HASHMAP_SIZE, strlen_hash,     database);
}

// ---------------------------------------------------------------------------------------------------------------------

void benchmarking_search(database_t *database) {
    hashmap_t *map = hashmap::ctor(HASHMAP_SIZE, crc32_intrin_hash);
    hashmap::populate(map, database);

    benchmark_search(map, database, REPORTS_DIR VERSION_NAME ".json");

    hashmap::dtor(map);
}

// ---------------------------------------------------------------------------------------------------------------------

#define benchmark_hash_macro(func) { \
    benchmark_hash(func, database, REPORTS_DIR #func "_time.json"); \
}

void benchmarking_hash(database_t *database) {
    benchmark_hash_macro(rol_hash);
    benchmark_hash_macro(ror_hash);
    benchmark_hash_macro(crc32_hash);
    benchmark_hash_macro(gnu_hash);
    benchmark_hash_macro(crc32_intrin_hash);
}