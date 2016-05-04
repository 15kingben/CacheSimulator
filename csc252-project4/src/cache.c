#include "cache.h"
#include "trace.h"



int write_xactions = 0;
int read_xactions = 0;
const int DEBUG = 0;

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

	int repPol = 0;

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
			repPol = 1;// = atoi(argv[i]);
    }
		//unrecognized input
		else{
			printf("Unrecognized argument. Exiting.\n");
			return -1;
		}
	}






	//run standard

	struct progOutput *fullOutput = runCache(size, line, (1024*size)/(line), filename, repPol );

	struct progOutput *standardOutput = runCache(size, line, ways, filename, repPol);



	int offsetBits = logBase2(line);
	int sets = (1024*size)/(line*ways);
	int indexBits = logBase2(sets);
	int tagBits = 32 - (indexBits + offsetBits);



	printf("Ways: %u; Sets: %u; Line Size: %uB\n", ways, sets, line); //
	printf("Tag: %d bits; Index: %d bits; Offset: %d bits\n", tagBits, indexBits, offsetBits);//
	/* Print results */
	printf("Miss Rate: %8lf%%\n", standardOutput->missRatio * 100.0);
	printf("Read Transactions: %d\n", standardOutput->totalReads);
	printf("Write Transactions: %d\n", standardOutput->totalWrites);



	char path[100] = "";

	strcat(path, filename);
	strcat(path, ".simulated");

	FILE *fp;


	fp = fopen(path, "w");




	int k = 0;
	for(k = 0; k < fullOutput->size; k++){
		fprintf(fp, "%s %s ", standardOutput->accessTypes[k], standardOutput->accessAdresses[k]);
		int fullResult = fullOutput->lines[k];
		int standResult = standardOutput->lines[k];
		if(standResult == 0){
			fprintf(fp, "hit\n");
		}else if(standResult == 2){
			fprintf(fp, "compulsory\n");
		}else if(fullResult == 1){
			fprintf(fp, "capacity\n");
		}else{
			fprintf(fp, "conflict\n");
		}


		//hit = 0 capacity = 1 compulsory = 2

	}
	fclose(fp);



	for(k = 0; k < 100; k++){//MAX_HISTORY_LENGTH
		free(fullOutput->accessAdresses[k]);
		free(fullOutput->accessTypes[k]);
	}
	free(fullOutput->accessAdresses);
	free(fullOutput->accessTypes);
	free(fullOutput->lines);
	free(fullOutput);

	for(k = 0; k < 100; k++){
		free(standardOutput->accessAdresses[k]);
		free(standardOutput->accessTypes[k]);
	}
	free(standardOutput->accessAdresses);
	free(standardOutput->accessTypes);
	free(standardOutput->lines);
	free(standardOutput);


	if(DEBUG){
		//char c = getchar();
	}







	if(DEBUG){
		printf("Dumping Tag History: \n");
		//int k;
		//for(k =0; i < cache->tagHistoryIndex; i++){
			//printf("%d\n", cache->tagHistory[i]);
		//}
	}

  /* TODO: Now we output the file */


}

int logBase2(int number){
	int log = 0;
	while(number >>= 1){
			log++;
	}
	return log;
}


int accessCache(struct Cache * cache, char * mode, char * address, int repPol, int *totalReads, int *totalWrites, int checkSeenBefore){
	if(repPol == 1){


		uint32_t intAddress = (int) strtol(address, NULL, 0);
		intAddress = intAddress >> (cache->offsetBits); //trash offset Bits
		uint32_t passAddress = intAddress;
		uint32_t index = intAddress & ((1 << (cache->indexBits)) - 1);
		intAddress = intAddress >> (cache->indexBits);
		uint32_t tag = intAddress & ((1 << (cache->tagBits)) - 1);


		int i;
		for(i = 0; i < cache->ways; i++){
			if(cache->valid[index][i] != -1 && cache->tags[index][i] == tag ){

				int j = i;
				int next = j + 1;
				if(next >= cache->ways){
					next = 0;
				}

				while(next != cache->setQueueMarker[index]){
					//swap j and next
					int temp = cache->tags[index][j];
					cache->tags[index][j] = cache->tags[index][next];
					cache->tags[index][next] = temp;

					//increment j + next & wraparound
					next += 1;
					j+= 1;
					if(next >= cache->ways){
						next = 0;
					}
					if(j >= cache->ways){
						j = 0;
					}

				}

				if(strcmp(mode, "s") == 0){
					cache->dirty[index][i] = 1; //if store change dirty bit
				}


				return 0; //cache hit
			}
		}

		int missType = 1;//capacity

		//CACHE MISS

		//target index to replace
		int replaceIndex = cache->setQueueMarker[index];

		//if dirty bit write back
		if(cache->dirty[index][replaceIndex]){
			(*totalWrites)++;//writeback to memory
		}

		//replace memory
		cache->tags[index][replaceIndex] = tag;

		if(checkSeenBefore){
			if(checkCompulsory(passAddress, cache)){
					missType = 2; //2: compulsory
			}
		}


		//if store set dirty bit 1
		if(strcmp(mode, "s") == 0){
			cache->dirty[index][replaceIndex] = 1; //if store change dirty bit
		}else{
			cache->dirty[index][replaceIndex] = 0;
		}
		(*totalReads)++;

		//increment index & wraparound
		cache->setQueueMarker[index] += 1;
		if(cache->setQueueMarker[index] >= cache->ways){
			cache->setQueueMarker[index] = 0;
		}

		//set valid bit 1
		cache->valid[index][replaceIndex] = 1;
		//cache->tagHistory[cache->tagHistoryIndex]  = passAddress;
		//(cache->tagHistoryIndex)  += 1;
		return missType;


	}else{

		uint32_t intAddress = (int) strtol(address, NULL, 0);
		intAddress = intAddress >> (cache->offsetBits); //trash offset Bits
		uint32_t passAddress = intAddress;
		uint32_t index = intAddress & ((1 << (cache->indexBits)) - 1);
		intAddress = intAddress >> (cache->indexBits);
		uint32_t tag = intAddress & ((1 << (cache->tagBits)) - 1);
		if(DEBUG){
			printf("Tag=%d", tag);
			printf("PassAddress=%d", passAddress);
		}

		int i;
		for(i = 0; i < cache->ways; i++){
			if(cache->valid[index][i] != -1 && cache->tags[index][i] == tag ){
				if(strcmp(mode, "s") == 0){
					cache->dirty[index][i] = 1; //if store change dirty bit
				}
				//cache->tagHistory[cache->tagHistoryIndex] = passAddress;
				//(cache->tagHistoryIndex)+=1;
				return 0; //cache hit
			}
		}

		int missType = 1;//capacity

		//CACHE MISS

		//target index to replace
		int replaceIndex = cache->setQueueMarker[index];

		//if dirty bit write back
		if(cache->dirty[index][replaceIndex]){
			(*totalWrites)++;//writeback to memory
		}

		//replace memory
		cache->tags[index][replaceIndex] = tag;

		// if(checkInHistory(passAddress, cache)){
		// 	missType = 3;//conflict
		// }
		if(checkSeenBefore){
			if(checkCompulsory(passAddress, cache)){
					missType = 2; //2: compulsory
			}
		}


		//if store set dirty bit 1
		if(strcmp(mode, "s") == 0){
			cache->dirty[index][replaceIndex] = 1; //if store change dirty bit
		}else{
			cache->dirty[index][replaceIndex] = 0;
		}
		(*totalReads)++;

		//increment index & wraparound
		cache->setQueueMarker[index] += 1;
		if(cache->setQueueMarker[index] >= cache->ways){
			cache->setQueueMarker[index] = 0;
		}

		//set valid bit 1
		cache->valid[index][replaceIndex] = 1;
		//cache->tagHistory[cache->tagHistoryIndex]  = passAddress;
		//(cache->tagHistoryIndex)  += 1;
		return missType;
	}
}


struct progOutput* runCache(int size, int line, int ways, char* filename, int repPol){
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
	cache->sets = sets;



	struct progOutput* ret = readFile(filename, cache, repPol, 1);


	int i;
	/* TODO: Cleanup */
	for(i = 0; i < cache->sets; i++){
		free(cache->valid[i]);
		free(cache->tags[i]);
		free(cache->dirty[i]);

	}
	free(cache->valid);
	free(cache->tags);
	free(cache->dirty);
	free(cache->seenBefore);
	free(cache->setQueueMarker);
	free(cache->tagHistory);
	free(cache);

	return ret;

}




int checkCompulsory(int tag, struct Cache * cache){
	int i = 0;
	for(i = 0; i < cache->seenBeforeLength; i++){
		if(tag == cache->seenBefore[i]){
			return 0;
		}
	}
	cache->seenBefore[cache->seenBeforeLength] = tag;
	++(cache->seenBeforeLength);
	return 1;
}



struct Cache* initCache(int sets, int ways){
	//Initialize cache data structures on heap
	struct Cache *cache = malloc(sizeof(struct Cache));


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

	//where we are in each set
	cache->setQueueMarker = malloc(sets * sizeof(int));
	for(i = 0; i < sets; i++){
		cache->setQueueMarker[i] = 0;
	}

	//the history for determining capacity uses
	cache->tagHistory = malloc(MAX_HISTORY_LENGTH * sizeof(int));
	cache->tagHistoryIndex = 0;

	cache->seenBefore = malloc(MAX_SEEN_BEFORE * sizeof(int));
	for(i = 0; i < MAX_SEEN_BEFORE; i++){
		cache->seenBefore[i] = 0;
	}

	//Set starting values for arrays
	int j = 0;
	for(i = 0; i < sets; i++){
		for(j = 0; j < ways; j++){
			cache->valid[i][j] = -1;
			cache->dirty[i][j] = 0;
			cache->tags[i][j] = 0;
		}
	}
	cache->seenBeforeLength = 0;
	return cache;
}
