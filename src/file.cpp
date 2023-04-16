#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include "file.h"

// ---------------------------------------------------------------------------------------------------------------------
// Internal structs
// ---------------------------------------------------------------------------------------------------------------------

struct mmaped_file_t {
    unsigned char *data;
    size_t size;
};

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

static mmaped_file_t mmap_file_or_warn(const char *name);
static void mmap_close (mmaped_file_t file);
static size_t get_file_size (int fd);

// ---------------------------------------------------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------------------------------------------------

result_t hashmap::populate(hashmap_t *self, const char *filename) {
    mmaped_file_t file = mmap_file_or_warn(filename);
    if (!file.data) { return result_t::ERROR; }

    char *key   = (char *) file.data;
    char *value = (char *) file.data;
    char *tmp   = nullptr;

    while (key - (const char *) file.data < file.size) {
        while (isspace(*key)) {key++;}

        value = strchr(key, ' ');
        *value = '\0';
        value++;
        while (isspace(*value)) {value++;}

        tmp = strchr(value, '\n');
        *tmp = '\0';

        insert(self, key, value);
        key = tmp+1;
    }

    return result_t::OK;
}

// ---------------------------------------------------------------------------------------------------------------------
// Private (static)
// ---------------------------------------------------------------------------------------------------------------------

static mmaped_file_t mmap_file_or_warn(const char *name) {
    int fd = open(name, O_RDWR);
    if (fd < 0) {
        fprintf (stderr, "Failed to open file '%s'\n", name);
        return {.data=nullptr};
    }

    size_t filesize = get_file_size(fd);

    unsigned char *mmap_memory = (unsigned char *) mmap(nullptr, filesize, PROT_READ | PROT_WRITE,
                                                        MAP_PRIVATE, fd, 0);

    if (mmap_memory == MAP_FAILED) {
        fprintf (stderr, "Failed to map memory\n");
        return {.data = nullptr};
    }

    return {.data=mmap_memory, .size=filesize};
}

//----------------------------------------------------------------------------------------------------------------------

static void mmap_close (mmaped_file_t file) {
    munmap(file.data, file.size);
}

//----------------------------------------------------------------------------------------------------------------------

static size_t get_file_size (int fd)
{
    assert (fd > 0 && "Invalid file descr");

    struct stat st = {};
    fstat(fd, &st);
    return st.st_size;
}