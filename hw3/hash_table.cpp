#include <cstdio>
#include <sys/mman.h>
#include <cstdlib>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include "hash_table.h"

void hash_table_init()
{
    FILE *f;

    if (access("hash_table.tmp", 0) < 0) {
        f = fopen("hash_table.tmp", "w+");
        fclose(f);
        truncate("hash_table.tmp", HASH_TABLE_SIZE * sizeof(hash_table_t));
    }

    int fd = open("hash_table.tmp", O_RDWR);
    struct stat statbuf;
    int err = fstat(fd, &statbuf);
    if(err){
        fprintf(stderr, "ERROR: unable to open '%s': %s\n\n", "hash_table.tmp", strerror(errno));
        exit(1);
    }

    hash_table = mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    close(fd);
}

void hash_table_put(unsigned long long key, unsigned int value){

}

int hash_table_get(unsigned long long key, unsigned int *value){

}

void hash_table_close(){
    int err = munmap(hash_table, statbuf.st_size);

    if(err){
        fprintf(stderr, "ERROR: unmao error: %s\n\n", strerror(errno));
        exit(1);
    }
}