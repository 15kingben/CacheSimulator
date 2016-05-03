#include "trace.h"

// Put anything for reading trace files / writing output files here.

/* TODO: read trace file function */
struct progOutput* readFile(char * filename, struct Cache * cache, char * repPol, int checkSeenBefore){
  FILE *ifp;
  ifp = fopen(filename, "r");
  char mode[2];
  char address[10];



  if (ifp == NULL) {
    fprintf(stderr, "Can't open input file\n");
    exit(1);
  }

  struct progOutput *output = (struct progOutput*)malloc(sizeof(struct progOutput));

  int k = 0;

  output->accessTypes = malloc((sizeof(char*)) * 1000000);
  output->accessAdresses = malloc(sizeof(char*) * 1000000);



  for(k = 0; k < 1000000; k++){
    output->accessAdresses[k] = malloc(sizeof(char) * 12);
    output->accessTypes[k] = malloc(sizeof(char) * 4);
  }

  output->lines = malloc(sizeof(int) * 1000000);
  output->size = 0;

  int totalReads = 0;
  int totalWrites = 0;

  float missCounter =0.0;
  float totalCounter = 0.0;

  while (fscanf(ifp, "%s %s", mode, address) != EOF) {
    printf("%s %s ", mode, address);

    int ret = accessCache(cache, mode, address, NULL, &totalReads, &totalWrites, checkSeenBefore);
    if(ret == 0){
      totalCounter += 1.0;
      output->lines[output->size] = 0;
      output->size+=1;
      //printf("hit\n");
    }else if(ret == 2){//compulsory
      missCounter += 1.0;
      totalCounter += 1.0;
      output->lines[output->size] = 2;
      output->size+=1;//
      //printf("compulsory\n");

    }else if(ret == 3){//conflict
      missCounter += 1.0;
      totalCounter += 1.0;
      output->lines[output->size] = 3;
      output->size+=1;
      //printf("conflict\n");
    }else {//capacity
      missCounter += 1.0;
      totalCounter += 1.0;
      output->lines[output->size] = 1;
      output->size+=1;
      //printf("capacity\n");
    }
    //if(	(int) strtol(address, NULL, 0) == (int) strtol("0x1ffffde8", NULL, 0) || (int) strtol(address, NULL, 0) == (int) strtol("0x1ffffe48", NULL, 0)){
    //  char c = getchar();
    //}
  }
  fclose(ifp);
  output->totalReads = totalReads;
  output->totalWrites = totalWrites;
  output->missRatio = (missCounter/totalCounter);
  //printf("Reads %d Writes %d Ratio %f\n", totalReads, totalWrites, );
  return output;
}






/* TODO: write output file function */
