#include <nmmintrin.h>
#include "hash.h"


static inline uint64_t rol(uint64_t byte);
static inline uint64_t ror(uint64_t byte);

// ---------------------------------------------------------------------------------------------------------------------

uint64_t gnu_hash(const char *obj)
{
    uint64_t hash = 5381;

    unsigned char c = 0;

    while ((c = (unsigned char) *obj++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t crc32_hash(const char *obj) {
    unsigned char byte = 0;
    unsigned int crc = 0xFFFFFFFF, mask = 0;

    while ((byte = (unsigned char) *obj++))
    {
        crc = crc ^ byte;

        for (int j = 7; j >= 0; j--)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
    }
    return ~crc;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t rol_hash(const char *obj) {
    uint64_t hash = 0;
    unsigned char c = 0;

    while ((c = (unsigned char) *obj++)) {
        hash = rol(hash) + c;
    }

    return hash;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ror_hash(const char *obj) {
    uint64_t hash = 0;
    unsigned char c = 0;

    while ((c = (unsigned char) *obj++)) {
        hash = ror(hash) + c;
    }

    return hash;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t const_hash(const char *) {
    return 22801337;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t first_char_hash(const char *obj) {
    return (uint64_t) obj[0];
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t sum_hash(const char *obj) {
    uint64_t hash = 0;
    unsigned char c = 0;

    while ((c = (unsigned char) *obj++)) {
        hash += c;
    }

    return hash;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t avg_hash(const char *obj) {
    uint64_t hash = 0;
    unsigned char c = 0;
    unsigned int len = 0;

    while ((c = (unsigned char) *obj++)) {
        hash += c;
        len++;
    }

    return hash / len;
}

// ---------------------------------------------------------------------------------------------------------------------
// Static
// ---------------------------------------------------------------------------------------------------------------------
static inline uint64_t rol(uint64_t byte) {
    return ((byte << 1)) | (byte >> 63);
}

static inline uint64_t ror(uint64_t byte) {
    return ((byte >> 1)) | (byte << 63);
}