/*
 * first.c
 *
 *  Created on: Apr 5, 2017
 *      Author: Neil
 */
#include "first.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define bitsize 4

/*
 * readOrWriteFullAssoc: takes a tag, a list and a cache and updates the values within the list and the cache
 * input: unsignedlong tag, R or W, a full associativity list, and a cache
 * output: fullAssocList
 */
fullList* readOrWriteFullAssoc(unsigned long long retTag, char readOrwrite,fullList *list, cache *cacheA){
	int size = list->size;
	if(readOrwrite == 'R'){
		for(int i = 0; i < size; i++){
			//read Hit
			if(list->numLines[i].tag == retTag && list->numLines[i].valid == 1){
				cacheA->hits += 1;
				return list;
			}
			//read cold miss
			if(list->numLines[i].valid == 0){
				cacheA->miss += 1;
				cacheA->reads += 1;
				list->numLines[i].valid = 1;
				list->numLines[i].tag = retTag;
				return list;
			}

		}
		cacheA->miss += 1;
		cacheA->reads +=1;
		for(int j = 0; j < size-1; j++){
			list->numLines[j].tag = list->numLines[j+1].tag;
		}
		list->numLines[size-1].tag = retTag;
		return list;
	}else{
		//is write
		for(int i = 0; i < size; i++){
			line *temp =  &list->numLines[i];
			//line is valid and its a HIT
			if(temp->valid == 1 && temp->tag == retTag){
				cacheA->writes += 1;
				cacheA->hits +=1;
				return list;
			}

			//no other tags contained it and there is a spot open
			if(temp->valid == 0){
				temp->valid = 1;
				temp->tag = retTag;
				cacheA->miss += 1;
				cacheA->reads += 1;
				cacheA->writes += 1;
				return list;
			}

		}
		//not found and no space left
		//must replace something
		//writeMiss++;
		cacheA->writes += 1;
		cacheA->miss +=1;
		cacheA->reads +=1;
		for(int j = 0; j < size-1; j++){
			list->numLines[j].tag = list->numLines[j+1].tag;
		}
		list->numLines[size-1].tag = retTag;

	}
	return list;
}

/*
 * createFullAssocList- intializes a List of lines A
 * input: structure A
 * output: structure A
 */
fullList* createFullAssocList(fullList *A){
	A->numLines = (line*) malloc(A->size * (sizeof(line)));
	for(int i = 0; i < A->size; i++){
		A->numLines[i].valid = 0;
		A->numLines[i].tag = 0;
	}
	return A;
}

/*
 *  createHashTableDN: creates a hashtable based on number of sets and lines, for either direct or n-way
 *  input:num lines per set, number of set and cache to populate
 *  output:hashTable
 */
hashTable* createHashTableDN(int linesPerSet, int sets, hashTable *hashInput){
	if(linesPerSet < 1){
		printf("Not enough lines per set\n");
		exit(0);
	}
	hashInput->size = sets;
	hashInput->linesPerSet = linesPerSet;
	hashInput->numLines = (line **) malloc(sets * (sizeof(line *)));
	for(int i = 0; i  < sets; i++){
		hashInput->numLines[i] = (line *) malloc(linesPerSet * (sizeof(line)));
		for(int j = 0; j <linesPerSet; j++){
			hashInput->numLines[i][j].valid = 0;
			hashInput->numLines[i][j].tag = 0;
		}
	}
	return hashInput;
}

/*
 * hashTableSearch: Does everything for direct/ nway assoc cache
 * input: line tag and set, hashTable to change, Read or Write, and Cache to change
 * output: hashTable
 */
hashTable* hashTableSearch(line *inputLine, hashTable *input,char readOrWrite, cache *inputCache){
	int size = input->size;
	unsigned long long set = inputLine->set;
	unsigned long long tag = inputLine->tag;
	unsigned long long key1 = set % (unsigned long long)size;
	int key = (int) key1;

	line *tempArray = input->numLines[key];
	int size2 = input->linesPerSet;
	if(readOrWrite == 'R'){
		//read
		for(int i = 0; i < size2; i++){
			//miss since nothings there yet
			if(tempArray[i].valid == 0){
				inputCache->reads += 1;
				inputCache->miss +=1;
				tempArray[i].valid = 1;
				tempArray[i].tag = tag;
				return input;
			}
			//hit
			if(tempArray[i].tag == tag){
				inputCache->hits += 1;
				return input;
			}
			//does not equal tag so must replace
			//direct mapped
			if(size2 == 1){
				inputCache->miss += 1;
				inputCache->reads +=1;
				tempArray->tag = tag;
				return input;
			}
		}
		//not direct map must replace not foundc
			inputCache->miss += 1;
			inputCache->reads +=1;
		for(int j = 0; j <size2-1; j++){
			tempArray[j].tag = tempArray[j+1].tag;
		}
			tempArray[size2-1].tag = tag;
			return input;
	}else{
		//write
		for(int i = 0; i < size2; i++){
				if(tempArray[i].valid == 1 && tempArray[i].tag == tag){
					//hit
					inputCache->writes += 1;
					inputCache->hits +=1;
					return input;
				}
				//no other tags contained it and there is a spot open
				if(tempArray[i].valid == 0){
					tempArray[i].valid = 1;
					tempArray[i].tag = tag;
					inputCache->miss += 1;
					inputCache->reads += 1;
					inputCache->writes += 1;
					return input;
				}
				//direct map cache
				if(size2 == 1){
					inputCache->miss += 1;
					inputCache->reads +=1;
					inputCache->writes +=1;
					tempArray->tag = tag;
					return input;
				}
			}
			//not found and no space left
			//must replace something
			inputCache->writes += 1;
			inputCache->miss +=1;
			inputCache->reads +=1;
		for(int j = 0; j < size2-1; j++){
			tempArray[j].tag = tempArray[j+1].tag;
		}
			tempArray[size2-1].tag = tag;
	}
	return input;
}//end function



/* findAssoc:
 * Input: associativeity from command line
 * Output: Integer value representing what type of cache to work with
 * -1 for direct mapped, 0 for fully associatve and > 0 and a factor 2 is returned
 */
int findAssoc(char assoc[]){
	if(strcmp(assoc,"direct") == 0){
		return -1;
	}else if((strcmp(assoc,"assoc") == 0)){
		return 0;
	}
	if(assoc[0]!= 'a'){
		printf("Error associativity\n");
		exit(0);
	}
	int length = strlen(assoc);
	char temp[length - 5];
	int j = 0;
	for(int i = 6; i < length; i++){
		temp[j] = assoc[i];
	}
	int temp2 = atoi(temp);
	if(temp2 == 1){
		return- 1;
	}
	if(temp2 % 2 != 0){
		printf("Error associativity: was not a power of 2\n");
		exit(0);
	}
	return temp2;
}

/*
 * numofSets: finds number of sets cache will require
 * input: cachesize(in bytes), blocksize(in bytes) and associativity
 * output: num of sets
 */
int numofSets(unsigned int cachesize, unsigned int blocksize, int assoc){
	if(blocksize > cachesize){
		printf("Block size cannot be bigger than cache! \n");
		exit(0);
	}
	//direct map cache
	if(assoc == -1){
		return cachesize/blocksize;
	}else if (assoc == 0){
		//E =  0
		//fully assoc cache
		//there are no sets
		return 0;
	}
	//E > 1
	//nway assoc cache
	int divider = blocksize * assoc;
	if(divider == 0){
		printf("Assocativity is too high\n");
		exit(0);
	}
	return cachesize/(divider);
}

/*
 * findNumLines: returns num of lines per SET
 * input = cachesize, blocksize and type of associativity
 *output:zero, 1 or num lines
 */
int findNumLines(int cachesize, int blocksize,int assoc){
 if(assoc== -1){
	 //direct map
	 return 1;
 }else if( assoc > 0){
	 //nway assoc
	 return assoc;
 }
  //fully assoc
 // cachesize = x * 1 *blocksize
  return cachesize / blocksize;
}


/*
 * findTagIndex: finds number of index bits and tag bits required
 *  tagIndex[0] = num tagbits  ( should always be number)
 *  tagIndex[1] = num of index bits(will be either number or -1 if fully assoc)
 *  tagIndex[3] = num of block bits
 * input: blocksize, cachesize, number of Sets, type of associativity, an array to hold the values
 * output: Nothing
 */
void findTagIndex(unsigned int blocksize ,unsigned int cachesize, int numSets,int assoc, int tagIndex[]){
	if(cachesize == 0 || blocksize ==0 ){
		printf("cache size or block size was 0");
		exit(0);
	}
	if(cachesize%2 != 0){
		printf("cache size was not a power of 2");
		exit(0);
	}
	if(blocksize != 1){
		if(blocksize %2 != 0){
			printf("cachesize was not a power of 2");
			exit(0);
		}
	}
	int tag = 0, index = 0;
	int blockbits = (int)log2(blocksize);
	if(assoc != 0){
		//direct map or n way associative
		index = (int)log2(numSets);
		tag = 48 - index - blockbits;
		tagIndex[0] = tag;
		tagIndex[1] = index;
		tagIndex[2] = blockbits;
		return;
	}
	//fully associative
	  tag = 48 - blockbits;
      index = 0;
      tagIndex[0] = tag;
      tagIndex[1] = index;
      tagIndex[2] = blockbits;
}

/*
 * printOutput: prints output of both cacheA and cache B to the command line
 * input: cache A and cache B
 * output: void
 */
void printOutput(cache *A, cache *B){
		printf("cache A\n");
		printf("Memory reads: %d\n",A->reads);
		printf("Memory writes: %d\n",A->writes);
		printf("Cache hits: %d\n",A->hits);
		printf("Cache misses: %d\n",A->miss);
		printf("cache B\n");
		printf("Memory reads: %d\n",B->reads);
		printf("Memory writes: %d\n",B->writes);
		printf("Cache hits: %d\n",B->hits);
		printf("Cache misses: %d\n",B->miss);
}
/*
 * callField: takes a memoryAddres and respecitvely shifts it depending on start and blockoffset size
 * input:unsigned long long memoryAddress, int start and int size
 * output:unsingned long long tag
 */
unsigned long long callField(unsigned long long memoryAddress, int start, int size){
	unsigned long long retTag;
	retTag= (memoryAddress >> (start)) & ((1ll << size) -1 );
	return retTag;
}

/*
 * convertHexToLong: Used in fullAssoc cache
 * input: memoryAddres and tagIndex[]
 * output: tag for fullAssoc
 */
unsigned long long convertHexToLong(unsigned long long memoryAddress, int tagIndex[]){
	//convert memoryAddress to tag
	unsigned long long retTag = 0;
	int t = tagIndex[0];
	int s = tagIndex[1];
	//should never happen
	if(s != 0){
		printf("set bits in full assoc is not equal to 0\n");
		exit(0);
	}
	int b = tagIndex[2];
	retTag = callField(memoryAddress, b, t);
	return retTag;
}

/*
 * convertHexToLine: used for direct and nway associatve caches to create line structures
 * input:memoryAddress from traceFile, tagIndex, line1 (A), line2(B)
 * output:nothing
 */
void convertHexToLine(unsigned long long memoryAddress,int tagIndex[], line *line1, line *line2){
	int t = tagIndex[0];
	int s = tagIndex[1];
	int b = tagIndex[2];
	//retTag = (memoryAddress >> (b + s)) & ((1ll << t) -1);
	//set = (memoryAddress >> (b)) & ((1ll << s) -1 );
	 //A is 47 - 0 (tag, index)
	 //B is 47-0 (index, tag)
	unsigned long long tagA = callField(memoryAddress, b+s, t);
	unsigned long long tagB = callField(memoryAddress, b, t);

	//will be direct or n way assoc
	unsigned long setA = callField (memoryAddress, b, s);
	unsigned long setB  = callField (memoryAddress, b+t, s);
	line1->set = setA;
	line2->set = setB;
	line1->tag = tagA;
	line2->tag = tagB;
}

int main(int argc, char** argv){
	//error checking
	if(argc != 5){
		printf("Error input\n!");
		exit(0);
	}
	//pre-setup
	unsigned int cachesize = atoi(argv[1]);
	//printf("Cache Size is = %u\n", cachesize);
	char* assoctivity = argv[2];
	if(strlen(assoctivity) < 5){
		printf("problem associativity");
		exit(0);
	}
	/*
	 * val will
	 * return -1 for direct map
	 * return 0 for full assoc
	 * return n for n way full assoc
	 */
	int val = findAssoc(assoctivity);
	unsigned int blocksize = atoi(argv[3]);
	//printf("Block Size is = %u\n", blocksize);
	int numSets = numofSets(cachesize, blocksize, val);
	int linesPerSet = findNumLines(cachesize,blocksize,val);

	/*
	 * tag index 0: numtag bits
	 * tag index 1: index bits
	 * tag index 3: blockbits
	 */
	int tagIndex[3];
	findTagIndex(blocksize, cachesize, numSets, val, tagIndex);
	char programCounter[30];
	char readOrwrite;
	unsigned long long memoryAddress = 0;
	FILE *traceFile= fopen(argv[4],"r");
	if(traceFile == NULL){
		printf("File does not exist!\n");
		exit(0);
	}

	//cache setup
	cache *cacheA = (cache *) malloc(sizeof(cache));
	cache *cacheB = (cache *) malloc(sizeof(cache));
	cacheA->hits = 0;
	cacheA->miss = 0;
	cacheA->reads = 0;
	cacheA->writes = 0;
	cacheB->hits = 0;
	cacheB->miss = 0;
	cacheB->reads = 0;
	cacheB->writes = 0;

	//structure set up
	fullList *fullListA;
	hashTable *hashTableA;
	hashTable *hashTableB;

	if(val == 0){
		//full associative
		fullListA = (fullList *) malloc(sizeof(fullList));
		fullListA->size = linesPerSet;
		createFullAssocList(fullListA);
	}else{
		//either direct or n-way associative
		hashTableA = (hashTable *) malloc(sizeof(hashTable));
		hashTableB = (hashTable *) malloc(sizeof(hashTable));
		hashTableA = createHashTableDN(linesPerSet,numSets,hashTableA);
		hashTableB = createHashTableDN(linesPerSet,numSets,hashTableB);
	}
	int checker = 0;
	while(fscanf(traceFile,"%s %c %llx", programCounter, &readOrwrite, &memoryAddress)){
		if(feof(traceFile)){
			break;
		}
		if((readOrwrite != 'R') && (readOrwrite != 'W')){
			printf("Bad read or write input\n");
			exit(0);
		}
		//fully assoc cache
		if(val == 0){
			unsigned long long retTag = 0;
			retTag = convertHexToLong(memoryAddress, tagIndex);
			//printf("RET TAG IS = %llx \n",retTag);
			fullListA = readOrWriteFullAssoc(retTag,readOrwrite,fullListA, cacheA);
			//printOutput(cacheA,cacheA);
		}
		//d-map  and n-assoc
		if(val!=0){
			line *line2= (line*) malloc(sizeof(line));
			line *line1 = (line*) malloc(sizeof(line));
			convertHexToLine(memoryAddress,tagIndex,line1, line2);
			hashTableA = hashTableSearch(line1,hashTableA,readOrwrite,cacheA);
			hashTableB = hashTableSearch(line2,hashTableB,readOrwrite,cacheB);
			free(line1);
			free(line2);
		}
		checker++;

	}

	//no need to run twice for A and B for full Assoc
	if(val == 0){
		cacheB->hits = cacheA->hits;
		cacheB->miss = cacheA->miss;
		cacheB->reads= cacheA->reads;
		cacheB->writes = cacheA->writes;
	}

	fclose(traceFile);
	printOutput(cacheA,cacheB);
	//Freeing all the data
	//free if full assoc
	if(val == 0){
				free(fullListA->numLines);


	free(fullListA);
	}

	//freeing if direct or nway assoc
	if(val != 0){
		for(int z = 0; z<hashTableA->size; z++){
			free(hashTableA->numLines[z]);
			free(hashTableB->numLines[z]);
		}
		free(hashTableA->numLines);
		free(hashTableB->numLines);
		free(hashTableA);
		free(hashTableB);
	}
	free(cacheA);
	free(cacheB);
	return 0;
}
