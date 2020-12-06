#include <libgen.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unordered_map>
using namespace std;

#define BLOCK_NUM 128
#define VAL_SIZE 128
#define TABLE_SIZE 10
#define MEM_OFFSET_BIT 7
#define MEM_OFFSET_CONST 0x7f
#define FILE_OFFSET_BIT 50

typedef struct {
    unsigned long long tag;
    bool dirty;
    unsigned int lru;
    char val[BLOCK_NUM][VAL_SIZE + 1];
} block_t;

int main(int argc, char *argv[])
{
    FILE *fin, *fout, *ftmp;
    unsigned long long key1, key2;
    unsigned long long tag;
    unsigned int offset;
    unsigned int fileName;
    unsigned int tableNum = 0;
    char instr[8];
    char buf[1024];
    char buf2[1024];
    block_t *memTable;
    unordered_map<unsigned long long, int> tableIndex;
    unordered_map<unsigned long long, int> fileIndex;

    memTable = (block_t *) calloc(TABLE_SIZE, sizeof(block_t));

    if (!(fin = fopen(argv[1], "r"))) {
        fprintf(stderr, "ERROR: unable to open '%s': %s\n\n", argv[1],
                strerror(errno));
        exit(1);
    }

    strncpy(buf, basename(argv[1]), strlen(basename(argv[1])) - 5);
    buf[strlen(basename(argv[1])) - 5] = '\0';
    strcat(buf, "output");

    if (!(fout = fopen(buf, "w+"))) {
        fprintf(stderr, "ERROR: unable to open '%s': %s\n\n", argv[1],
                strerror(errno));
        exit(1);
    }

    while (fscanf(fin, "%s", instr) != EOF) {
        if (!strcmp(instr, "PUT")) {
            fscanf(fin, "%llu %s", &key1, buf);
            tag = key1 >> MEM_OFFSET_BIT;
            offset = key1 & MEM_OFFSET_CONST;
            fileName = tag >> FILE_OFFSET_BIT;
            sprintf(buf, "storage/%d.tmp", fileName);
            if (access(buf, 0) < 0) {
                ftmp = fopen(buf, "w+");
                fprintf(ftmp, "0                   \n");
                fclose(ftmp);
            }
            if (!tableIndex[tag]) {
                if (tableNum < TABLE_SIZE) {
                    tableIndex[tag] = ++tableNum;
                }
                else {
                    if (!(ftmp = fopen(buf, "r+"))) {
                        fprintf(stderr, "ERROR: unable to open '%s': %s\n\n",
                                argv[1], strerror(errno));
                        exit(1);
                    }
                    if (!fileIndex[tag]) {
                        unsigned long long fileSize;
                        fscanf(ftmp, "%llu", &fileSize);
                        fileIndex[tag] = ++fileSize;
                        fprintf(ftmp, "-20%llu", fileSize);
                        fwrite()
                    }

                    fclose(ftmp);
                }
            }
            memTable[tableIndex[tag] - 1].val[offset][0] = '1';
            memcpy(memTable[tableIndex[tag] - 1].val[offset] + 1, buf,
                   VAL_SIZE);
            memTable[tableIndex[tag] - 1].dirty = 1;
        }
        else if (!strcmp(instr, "GET")) {
            fscanf(fin, "%llu", &key1);
            tag = key1 >> MEM_OFFSET_BIT;
            offset = key1 & MEM_OFFSET_CONST;
            if (!tableIndex[tag]) {
            }
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
                if (!tableIndex[tag]) {
                }
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

    free(memTable);
    fclose(fin);
    fclose(fout);
    return 0;
}
