#include "trace.h"

// Put anything for reading trace files / writing output files here.

/* TODO: read trace file function */
void readFile(char * filename, struct Cache * cache, char * repPol){
  FILE *ifp;
  ifp = fopen(filename, "r");
  char mode[2];
  char address[10];

  if (ifp == NULL) {
    fprintf(stderr, "Can't open input file\n");
    exit(1);
  }

  int totalReads = 0;
  int totalWrites = 0;

  while (fscanf(ifp, "%s %s", mode, address) != EOF) {
    printf("Mode: %s  Address: %s ", mode, address);
    int ret = accessCache(cache, mode, address, NULL, &totalReads, &totalWrites);
    if(ret){
      printf("hit\n");
    }else{
      printf("miss\n");
    }
  }



}





/* TODO: write output file function */
