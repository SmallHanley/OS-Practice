#include "hash_table.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

hash_table_t hash_table;
FILE *f;

void hash_table_init()
{
    if (access("storage/hash_table.tmp", 0) < 0) {
        f = fopen("storage/hash_table.tmp", "w+");
        fclose(f);
        truncate("storage/hash_table.tmp",
                 HASH_TABLE_SIZE * sizeof(hash_table_t));
    }

    f = fopen("storage/hash_table.tmp", "r+");
}

void hash_table_put(unsigned long long key, unsigned int value)
{
    int index = key % HASH_TABLE_SIZE;
    fseek(f, index * sizeof(hash_table_t), SEEK_SET);
    fread(&hash_table, 1, sizeof(hash_table_t), f);

    while (hash_table.valid) {
        index = (index + 1) % HASH_TABLE_SIZE;
        fread(&hash_table, 1, sizeof(hash_table_t), f);
    }

    hash_table.valid = true;
    hash_table.key = key;
    hash_table.value = value;

    fseek(f, -1 * sizeof(hash_table_t), SEEK_CUR);
    fwrite(&hash_table, 1, sizeof(hash_table_t), f);
}

int hash_table_get(unsigned long long key, unsigned int *value)
{
    int index = key % HASH_TABLE_SIZE;
    fseek(f, index * sizeof(hash_table_t), SEEK_SET);
    fread(&hash_table, 1, sizeof(hash_table_t), f);

    while (hash_table.valid) {
        if (hash_table.key == key) {
            *value = hash_table.value;
            return 0;
        }
        else {
            index = (index + 1) % HASH_TABLE_SIZE;
            fread(&hash_table, 1, sizeof(hash_table_t), f);
        }
    }

    return -1;
}

void hash_table_close()
{
    fclose(f);
}