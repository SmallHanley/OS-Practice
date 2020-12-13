#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <sys/stat.h>

#define HASH_TABLE_SIZE 100000000

typedef struct hash_table_t {
    bool valid;
    unsigned long long key;
    unsigned int value;
} hash_table_t;

void hash_table_init();

void hash_table_put(unsigned long long key, unsigned int value);

int hash_table_get(unsigned long long key, unsigned int *value);

void hash_table_close();

#endif  // HASH_TABLE_H