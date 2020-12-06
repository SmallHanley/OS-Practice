#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>

typedef struct hash_table_t {
	bool valid;
	unsigned long long key;
	unsigned int value;
}hash_table_t;

void hash_table_init();

void put(unsigned long long key, unsigned int value);

int get(unsigned long long key);

#endif // HASH_TABLE_H
