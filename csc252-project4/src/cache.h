#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

extern int write_xactions;
extern int read_xactions;


void printHelp(const char * prog);
int logBase2(int);
struct Cache* initCache(int, int);
int accessCache(struct Cache*, char*, char*, char*, int *, int *);


#ifndef TYPES
#define TYPES
struct Cache {
		int **valid;//booleans
		int **dirty;//booleans
		int **tags;
    int *setQueueMarker;
    int offsetBits;
    int indexBits;
    int tagBits;
    int ways;
};

enum accessType {READ, WRITE};
enum replacementPolicy {FIFO, LRU};
#endif
