#include <stdio.h>

#include "common.h"
#include "hashmap.h"
#include "file.h"
#include "benchmark.h"

const int HASHMAP_SIZE = 10000;
const char DICTIONARY_FILE[] = "data/dictfull.txt";

int main() {
    gen_hash_stats(HASHMAP_SIZE, gnu_hash, DICTIONARY_FILE);
}