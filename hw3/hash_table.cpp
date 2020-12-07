#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "hash_table.h"

hash_table_t *hash_table;
struct stat statbuf;

void hash_table_init()
{
    FILE *f;

    if (access("storage/hash_table.tmp", 0) < 0) {
        f = fopen("storage/hash_table.tmp", "w+");
        fclose(f);
        truncate("storage/hash_table.tmp", HASH_TABLE_SIZE * sizeof(hash_table_t));
    }

    int fd = open("storage/hash_table.tmp", O_RDWR);

    int err = fstat(fd, &statbuf);
    if (err) {
        fprintf(stderr, "ERROR: unable to open '%s': %s\n\n", "hash_table.tmp",
                strerror(errno));
        exit(1);
    }

    hash_table = (hash_table_t *) mmap(
        NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    close(fd);
}

void hash_table_put(unsigned long long key, unsigned int value)
{
    int index = key % HASH_TABLE_SIZE;
    while (hash_table[index].valid) {
        index = (index + 1) % HASH_TABLE_SIZE;
    }
    hash_table[index].valid = true;
    hash_table[index].key = key;
    hash_table[index].value = value;
}

int hash_table_get(unsigned long long key, unsigned int *value)
{
    int index = key % HASH_TABLE_SIZE;
    while (hash_table[index].valid) {
        if (hash_table[index].key == key) {
            *value = hash_table[index].value;
            return 0;
        }
        else {
            index = (index + 1) % HASH_TABLE_SIZE;
        }
    }
    return -1;
}

void hash_table_close()
{
    int err = munmap(hash_table, statbuf.st_size);

    if (err) {
        fprintf(stderr, "ERROR: unmao error: %s\n\n", strerror(errno));
        exit(1);
    }
}