#include "cache.h"
#include "trace.h"

int write_xactions = 0;
int read_xactions = 0;

/*
  Print help message to user
*/
void printHelp(const char * prog) {
	printf("%s [-h] | [-s <size>] [-w <ways>] [-l <line>] [-t <trace>]\n", prog);
	printf("options:\n");
	printf("-h: print out help text\n");
	printf("-s <cache size>: set the total size of the cache in KB\n");
	printf("-w <ways>: set the number of ways in each set\n");
	printf("-l <line size>: set the size of each cache line in bytes\n");
	printf("-t <trace>: use <trace> as the input file for memory traces\n");
  /* EXTRA CREDIT
  printf("-lru: use LRU replacement policy instead of FIFO\n");
  */
}
/*
	Main function. Feed to options to set the cache

	Options:
	-h : print out help message
	-s : set L1 cache Size (B)
	-w : set L1 cache ways
	-l : set L1 cache line size
*/
int main(int argc, char* argv[])
{
	int i;
	uint32_t size = 32; //total size of L1$ (KB)
	uint32_t ways = 1; //# of ways in L1. Default to direct-mapped
	uint32_t line = 32; //line size (B)

  // hit and miss counts
  int totalHits = 0;
  int totalMisses = 0;

  char * filename;

	//strings to compare
	const char helpString[] = "-h";
	const char sizeString[] = "-s";
	const char waysString[] = "-w";
	const char lineString[] = "-l";
	const char traceString[] = "-t";
  const char lruString[] = "-lru";

  if (argc == 1) {
    // No arguments passed, show help
    printHelp(argv[0]);
    return 1;
  }

	//parse command line
	for(i = 1; i < argc; i++)
	{
		//check for help
		if(!strcmp(helpString, argv[i]))
		{
			//print out help text and terminate
      printHelp(argv[0]);
			return 1; //return 1 for help termination
		}
		//check for size
		else if(!strcmp(sizeString, argv[i]))
		{
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if(isdigit(argv[i][0]))
			{
				size = atoi(argv[i]);
			}
			else
			{
				printf("Incorrect formatting of size value\n");
				return -1; //input failure
			}
		}
		//check for ways
		else if(!strcmp(waysString, argv[i]))
		{
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if(isdigit(argv[i][0]))
			{
				ways = atoi(argv[i]);
			}
			else
			{
				printf("Incorrect formatting of ways value\n");
				return -1; //input failure
			}
		}
		//check for line size
		else if(!strcmp(lineString, argv[i]))
		{
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if(isdigit(argv[i][0]))
			{
				line = atoi(argv[i]);
			}
			else
			{
				printf("Incorrect formatting of line size value\n");
				return -1; //input failure
			}
		}
    else if (!strcmp(traceString, argv[i])) {
      filename = argv[++i];
    }
    else if (!strcmp(lruString, argv[i])) {
      // Extra Credit: Implement Me!
			printf("Unrecognized argument. Exiting.\n");
			return -1;
    }
		//unrecognized input
		else{
			printf("Unrecognized argument. Exiting.\n");
			return -1;
		}
	}


	int offsetBits = logBase2(line);
	int sets = (1024*size)/(line*ways);
	int indexBits = logBase2(sets);
	int tagBits = 32 - (indexBits + offsetBits);


  /* : Probably should intitalize the cache */
	struct Cache *cache = initCache(sets, ways);
	cache->offsetBits = offsetBits;
	cache->indexBits = indexBits;
	cache->tagBits = tagBits;
	cache->ways = ways;

  printf("Ways: %u; Sets: %u; Line Size: %uB\n", ways, sets, line); //
  printf("Tag: %d bits; Index: %d bits; Offset: %d bits\n", tagBits, indexBits, offsetBits);//
	char c = getchar();

	/* TODO: Now we read the trace file line by line */
	readFile(filename, cache, NULL);


  /* TODO: Now we simulate the cache */





  /* Print results */
  printf("Miss Rate: %8lf%%\n", ((double) totalMisses) / ((double) totalMisses + (double) totalHits) * 100.0);
  printf("Read Transactions: %d\n", read_xactions);
  printf("Write Transactions: %d\n", write_xactions);

  /* TODO: Now we output the file */

  /* TODO: Cleanup */
}

int logBase2(int number){
	int log = 0;
	while(number >>= 1){
			log++;
	}
	return log;
}


int accessCache(struct Cache * cache, char * mode, char * address, char * repPol, int *totalReads, int *totalWrites){
	if(repPol != NULL){
		//Implement
	}else{
		int intAddress = (int) strtol(address, NULL, 0);
		intAddress = intAddress >> (cache->offsetBits); //trash offset Bits
		int index = intAddress & ((1 << (cache->indexBits)) - 1);
		intAddress = intAddress >> (cache->indexBits);
		int tag = intAddress & ((1 << (cache->tagBits)) - 1);



		int i;
		for(i = 0; i < cache->ways; i++){
			if(cache->valid[index][i] != -1 && cache->tags[index][i] == tag ){
				if(strcmp(mode, "s")){
					cache->dirty[index][i] = 1; //if store change dirty bit
				}
				return 1; //cache hit
			}
		}


		//cache miss
		int replaceIndex = cache->setQueueMarker[index];
		if(cache->dirty[index][replaceIndex]){
			(*totalWrites)++;//writeback to memory
		}

		cache->tags[index][replaceIndex] = tag;
		if(strcmp(mode, "s")){
			cache->dirty[index][replaceIndex] = 1; //if store change dirty bit
		}
		(*totalReads)++;

		cache->setQueueMarker[index] += 1;
		if(cache->setQueueMarker[index] >= cache->ways){
			cache->setQueueMarker[index] = 0;
		}
		cache->valid[index][i] = 1;
		return 0;
	}
}





struct Cache* initCache(int sets, int ways){
	//Initialize cache data structures on heap
	struct Cache *cache = (struct Cache*)malloc(sizeof(struct Cache));


	(cache)->valid = malloc(sets * sizeof(int *));

	int i = 0;
	for(i = 0; i < sets; i++){
		cache->valid[i] = malloc(ways * sizeof(int));
	}

	cache->dirty = malloc(sets * sizeof(int *));
	for(i = 0; i < sets; i++){
		cache->dirty[i] = malloc(ways * sizeof(int));
	}

	cache->tags = malloc(sets * sizeof(int *));
	for(i = 0; i < sets; i++){
		cache->tags[i] = malloc(ways * sizeof(int));
	}

	cache->setQueueMarker = malloc(sets * sizeof(int));

	//Set starting values for arrays
	int j = 0;
	for(i = 0; i < sets; i++){
		for(j = 0; j < ways; j++){
			cache->valid[i][j] = -1;
			cache->dirty[i][j] = 0;
			cache->tags[i][j] = 0;
		}
	}
	return cache;
}
