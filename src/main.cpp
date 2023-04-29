#include <stdio.h>

#include "common.h"
#include "hashmap.h"
#include "file.h"
#include "benchmark.h"

// ---------------------------------------------------------------------------------------------------------------------

const int HASHMAP_SIZE = 100000;
const char DICTIONARY_FILE[] = "data/dictbig.txt";

// ---------------------------------------------------------------------------------------------------------------------

void generate_statistics(database_t *database);
void benchmarking_search(database_t *database);

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
        gen_hash_stats(HASHMAP_SIZE, crc32_intrin_hash, database);
}

// ---------------------------------------------------------------------------------------------------------------------

void benchmarking_search(database_t *database) {
    hashmap_t *map = hashmap::ctor(HASHMAP_SIZE, crc32_intrin_hash);
    hashmap::populate(map, database);

    benchmark_search(map, database, REPORTS_DIR VERSION_NAME ".json");

    hashmap::dtor(map);
}
