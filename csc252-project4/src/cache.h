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
int accessCache(struct Cache *, char*, char*, int, int *, int *, int);
int checkCompulsory(int , struct Cache * );
struct progOutput* runCache(int , int , int, char*, int);


#ifndef TYPES
#define TYPES

static const int MAX_SEEN_BEFORE = 200000;
static const int MAX_HISTORY_LENGTH = 1000000;


struct progOutput {
	int *lines;
	int size;
	int totalReads;
	int totalWrites;
	double missRatio;
	char** accessTypes;
	char** accessAdresses;
};

struct Cache {
		int **valid;//booleans
		int **dirty;//booleans
		int **tags;
    int *setQueueMarker;
		int *seenBefore;
		int *tagHistory;
		int tagHistoryIndex;
		int seenBeforeLength;
    int offsetBits;
    int indexBits;
    int tagBits;
    int ways;
		int sets;
};

enum accessType {READ, WRITE};
enum replacementPolicy {FIFO, LRU};
#endif
