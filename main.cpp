#include <stdio.h>

#include "hashmap.h"

const int HASHMAP_SIZE = 10000;

int main() {
    hashmap_t *map = hashmap::ctor(HASHMAP_SIZE, gnu_hash);

    hashmap::insert(map, "hello", "world");
    printf ("%s\n", hashmap::find(map, "hello"));

    hashmap::dtor(map);
}