#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unordered_map>
#include "hash_table.h"
using namespace std;

#define BLOCK_NUM 128
#define VAL_SIZE 128
#define TABLE_SIZE 1
#define MEM_OFFSET_BIT 7
#define MEM_OFFSET_CONST 0x7f
#define FILE_OFFSET_BIT 50

typedef struct block_t {
    bool dirty;
    unsigned long long tag;
    char val[BLOCK_NUM][VAL_SIZE + 1];
} block_t;

int find_file_pos(unsigned long long tag, unsigned int *pos);

int main(int argc, char *argv[])
{
    FILE *fin, *fout, *ftmp1, *ftmp2;
    unsigned long long key1, key2;
    unsigned long long tag;
    unsigned int offset;
    unsigned int fileName;
    unsigned int tableNum = 0;
    unsigned int pos;
    char instr[8];
    char inputBuf[1024];
    char buf[1024];
    char buf2[1024];
    block_t *memTable;
    unordered_map<unsigned long long, int> tableIndex;

    memTable = (block_t *) calloc(TABLE_SIZE, sizeof(block_t));

    srand(time(NULL));

    hash_table_init();

    if (!(fin = fopen(argv[1], "r"))) {
        fprintf(stderr, "ERROR: unable to open '%s': %s\n\n", argv[1],
                strerror(errno));
        exit(1);
    }

    strncpy(buf, basename(argv[1]), strlen(basename(argv[1])) - 5);
    buf[strlen(basename(argv[1])) - 5] = '\0';
    strcat(buf, "output");

    if (!(fout = fopen(buf, "w+"))) {
        fprintf(stderr, "ERROR: unable to open '%s': %s\n\n", buf,
                strerror(errno));
        exit(1);
    }

    while (fscanf(fin, "%s", instr) != EOF) {
        if (!strcmp(instr, "PUT")) {
            fscanf(fin, "%llu %s", &key1, inputBuf);
            tag = key1 >> MEM_OFFSET_BIT;
            offset = key1 & MEM_OFFSET_CONST;
            fileName = tag >> FILE_OFFSET_BIT;
            sprintf(buf, "storage/%u.tmp", fileName);
            if (access(buf, 0) < 0) {
                ftmp1 = fopen(buf, "w+");
                fprintf(ftmp1, "0                   \n");
                fclose(ftmp1);
            }
            if (!tableIndex[tag]) {
                if (tableNum < TABLE_SIZE) {
                    tableIndex[tag] = ++tableNum;
                }
                else {
                    if (!(ftmp1 = fopen(buf, "r+"))) {
                        fprintf(stderr, "ERROR: unable to open '%s': %s\n\n",
                                buf, strerror(errno));
                        exit(1);
                    }
                    int discard = rand() % TABLE_SIZE;
                    tableIndex.erase(memTable[discard].tag);
                    if (memTable[discard].dirty) {
                        unsigned int fileName2 =
                            memTable[discard].tag >> FILE_OFFSET_BIT;
                        sprintf(buf2, "storage/%u.tmp", fileName2);
                        if (!(ftmp2 = fopen(buf2, "r+"))) {
                            fprintf(stderr,
                                    "ERROR: unable to open '%s': %s\n\n", buf2,
                                    strerror(errno));
                            exit(1);
                        }
                        fscanf(ftmp2, "%u", &pos);
                        if (find_file_pos(memTable[discard].tag, &pos)) {
                            fseek(ftmp2, 0, SEEK_SET);
                            fprintf(ftmp2, "%-20u\n", pos + 1);
                        }
                        fseek(ftmp2, 21 + pos * BLOCK_NUM * (VAL_SIZE + 1),
                              SEEK_SET);
                        fwrite(memTable[discard].val, 1,
                               BLOCK_NUM * (VAL_SIZE + 1), ftmp2);
                        fclose(ftmp2);
                    }
                    fscanf(ftmp1, "%u", &pos);
                    if (find_file_pos(tag, &pos)) {
                        fseek(ftmp1, 0, SEEK_SET);
                        fprintf(ftmp1, "%-20u\n", pos + 1);
                        truncate(buf, 21 + (pos + 1) * BLOCK_NUM * (VAL_SIZE + 1));
                    }
                    fseek(ftmp1, 21 + pos * BLOCK_NUM * (VAL_SIZE + 1), SEEK_SET);
                    fread(memTable[discard].val, (VAL_SIZE + 1), BLOCK_NUM, ftmp1);
                    fclose(ftmp1);
                    tableIndex[tag] = discard + 1;
                }
                memTable[tableIndex[tag] - 1].tag = tag;
            }
            memTable[tableIndex[tag] - 1].val[offset][0] = '1';
            memcpy(memTable[tableIndex[tag] - 1].val[offset] + 1, inputBuf,
                   VAL_SIZE);
            memTable[tableIndex[tag] - 1].dirty = true;
        }
        else if (!strcmp(instr, "GET")) {
            fscanf(fin, "%llu", &key1);
            tag = key1 >> MEM_OFFSET_BIT;
            offset = key1 & MEM_OFFSET_CONST;
            fileName = tag >> FILE_OFFSET_BIT;
            sprintf(buf, "storage/%u.tmp", fileName);
            if (access(buf, 0) < 0) {
                ftmp1 = fopen(buf, "w+");
                fprintf(ftmp1, "0                   \n");
                fclose(ftmp1);
            }
            if (!tableIndex[tag]) {
                if (!(ftmp1 = fopen(buf, "r+"))) {
                    fprintf(stderr, "ERROR: unable to open '%s': %s\n\n",
                            buf, strerror(errno));
                    exit(1);
                }
                int discard = rand() % TABLE_SIZE;
                tableIndex.erase(memTable[discard].tag);
                if (memTable[discard].dirty) {
                    unsigned int fileName2 = memTable[discard].tag >> FILE_OFFSET_BIT;
                    sprintf(buf2, "storage/%u.tmp", fileName2);
                    if (!(ftmp2 = fopen(buf2, "r+"))) {
                        fprintf(stderr, "ERROR: unable to open '%s': %s\n\n", buf2, strerror(errno));
                        exit(1);
                    }
                    fscanf(ftmp2, "%u", &pos);
                    if (find_file_pos(memTable[discard].tag, &pos)) {
                        fseek(ftmp2, 0, SEEK_SET);
                        fprintf(ftmp2, "%-20u\n", pos + 1);
                    }
                    fseek(ftmp2, 21 + pos * BLOCK_NUM * (VAL_SIZE + 1), SEEK_SET);
                    fwrite(memTable[discard].val, 1, BLOCK_NUM * (VAL_SIZE + 1), ftmp2);
                    fclose(ftmp2);
                }
                fscanf(ftmp1, "%u", &pos);
                if (find_file_pos(tag, &pos)) {
                    fseek(ftmp1, 0, SEEK_SET);
                    fprintf(ftmp1, "%-20u\n", pos + 1);
                    truncate(buf, 21 + (pos + 1) * BLOCK_NUM * (VAL_SIZE + 1));
                }
                fseek(ftmp1, 21 + pos * BLOCK_NUM * (VAL_SIZE + 1), SEEK_SET);
                fread(memTable[discard].val, 1, BLOCK_NUM * (VAL_SIZE + 1), ftmp1);
                fclose(ftmp1);
                tableIndex[tag] = discard + 1;
                memTable[discard].dirty = false;
            }
            memTable[tableIndex[tag] - 1].tag = tag;
            if (memTable[tableIndex[tag] - 1].val[offset][0] == '1') {
                fwrite(memTable[tableIndex[tag] - 1].val[offset] + 1, 1,
                       VAL_SIZE, fout);
            }
            else {
                fprintf(fout, "EMPTY");
            }
            fprintf(fout, "\n");
        }
        else if (!strcmp(instr, "SCAN")) {
            fscanf(fin, "%llu %llu", &key1, &key2);
            for (unsigned long long key = key1; key <= key2; key++) {
                tag = key >> MEM_OFFSET_BIT;
                offset = key & MEM_OFFSET_CONST;
                fileName = tag >> FILE_OFFSET_BIT;
                sprintf(buf, "storage/%u.tmp", fileName);
                if (access(buf, 0) < 0) {
                    ftmp1 = fopen(buf, "w+");
                    fprintf(ftmp1, "0                   \n");
                    fclose(ftmp1);
                }
                if (!tableIndex[tag]) {
                    if (!(ftmp1 = fopen(buf, "r+"))) {
                        fprintf(stderr, "ERROR: unable to open '%s': %s\n\n",
                                buf, strerror(errno));
                        exit(1);
                    }
                    int discard = rand() % TABLE_SIZE;
                    tableIndex.erase(memTable[discard].tag);
                    if (memTable[discard].dirty) {
                        unsigned int fileName2 = memTable[discard].tag >> FILE_OFFSET_BIT;
                        sprintf(buf2, "storage/%u.tmp", fileName2);
                        if (!(ftmp2 = fopen(buf2, "r+"))) {
                            fprintf(stderr, "ERROR: unable to open '%s': %s\n\n", buf2, strerror(errno));
                            exit(1);
                        }
                        fscanf(ftmp2, "%u", &pos);
                        if (find_file_pos(memTable[discard].tag, &pos)) {
                            fseek(ftmp2, 0, SEEK_SET);
                            fprintf(ftmp2, "%-20u\n", pos + 1);
                        }
                        fseek(ftmp2, 21 + pos * BLOCK_NUM * (VAL_SIZE + 1), SEEK_SET);
                        fwrite(memTable[discard].val, 1, BLOCK_NUM * (VAL_SIZE + 1), ftmp2);
                        fclose(ftmp2);
                    }
                    fscanf(ftmp1, "%u", &pos);
                    if (find_file_pos(tag, &pos)) {
                        fseek(ftmp1, 0, SEEK_SET);
                        fprintf(ftmp1, "%-20u\n", pos + 1);
                        truncate(buf, 21 + (pos + 1) * BLOCK_NUM * (VAL_SIZE + 1));
                    }
                    fseek(ftmp1, 21 + pos * BLOCK_NUM * (VAL_SIZE + 1), SEEK_SET);
                    fread(memTable[discard].val, 1, BLOCK_NUM * (VAL_SIZE + 1), ftmp1);
                    fclose(ftmp1);
                    tableIndex[tag] = discard + 1;
                    memTable[discard].dirty = false;
                }
                memTable[tableIndex[tag] - 1].tag = tag;
                if (memTable[tableIndex[tag] - 1].val[offset][0] == '1') {
                    fwrite(memTable[tableIndex[tag] - 1].val[offset] + 1, 1,
                        VAL_SIZE, fout);
                }
                else {
                    fprintf(fout, "EMPTY");
                }
                fprintf(fout, "\n");
            }
        }
        else {
            fprintf(stderr, "ERROR: instruction not found\n\n");
            exit(1);
        }
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (memTable[i].dirty) {
            unsigned int fileName2 = memTable[i].tag >> FILE_OFFSET_BIT;
            unsigned int pos;
            sprintf(buf2, "storage/%d.tmp", fileName2);
            if (!(ftmp2 = fopen(buf2, "r+"))) {
                fprintf(stderr, "ERROR: unable to open '%s': %s\n\n", buf2,
                        strerror(errno));
                exit(1);
            }
            fscanf(ftmp2, "%u", &pos);
            if (find_file_pos(memTable[i].tag, &pos)) {
                fseek(ftmp2, 0, SEEK_SET);
                fprintf(ftmp2, "%-20u\n", pos + 1);
            }
            fseek(ftmp2, 21 + pos * BLOCK_NUM * (VAL_SIZE + 1), SEEK_SET);
            fwrite(memTable[i].val, 1, BLOCK_NUM * (VAL_SIZE + 1), ftmp2);
            fclose(ftmp2);
        }
    }

    hash_table_close();
    free(memTable);
    fclose(fin);
    fclose(fout);
    return 0;
}

int find_file_pos(unsigned long long tag, unsigned int *pos)
{
    unsigned int value;
    if (hash_table_get(tag, &value)) {
        hash_table_put(tag, *pos);
        return -1;
    }
    else {
        *pos = value;
        return 0;
    }
}