#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include "file.h"

// ---------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------

static mmaped_file_t mmap_file_or_warn(const char *name);
static void mmap_close (mmaped_file_t file);
static size_t get_file_size (int fd);
static uint count_lines(const char *data);

// ---------------------------------------------------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------------------------------------------------

void hashmap::populate(hashmap_t *self, database_t *database) {
    for (uint i = 0; i < database->len; ++i) {
        insert(self, database->keys[i], database->values[i]);
    }
}

//----------------------------------------------------------------------------------------------------------------------

database_t *database::load(const char *dictionary_file) {
    mmaped_file_t file = mmap_file_or_warn(dictionary_file);
    if (!file.data) { return nullptr; }

    uint lines = count_lines((char *)file.data);
    database_t *self = (database_t *) calloc(1, sizeof(database_t));
    self->keys   = (char **) calloc(lines, sizeof(char *));
    self->values = (char **) calloc(lines, sizeof(char *));
    self->file   = file;

    char *key   = (char *) file.data;
    char *value = (char *) file.data;
    char *tmp   = nullptr;

    uint i = 0;
    for (; key - (char *)file.data < file.size; ++i) {
        while (isspace(*key)) { key++; }

        value = strchr(key, ' ');
        *value = '\0';
        value++;
        while (isspace(*value)) { value++; }

        tmp = strchr(value, '\n');
        *tmp = '\0';

        self->keys[i]   = key;
        self->values[i] = value;
        key = tmp + 1;
    }
    self->len = i;

    return self;
}

void database::unload(database_t *self) {
    mmap_close(self->file);
    free(self->values);
    free(self->keys);
    free(self);
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

//----------------------------------------------------------------------------------------------------------------------

static uint count_lines(const char *data) {
    uint counter = 0;
    data--;

    while ((data = strchr(data+1, '\n'))) {
        counter++;
    }

    return counter;
}