/*
 * first.h
 *
 *  Created on: Apr 5, 2017
 *      Author: Neil
 */

#ifndef FIRST_H_
#define FIRST_H_
#define MaxArraySize 48
#define bitsize 4

/*
 * creating a stucture called cache to hold on all the values for each type of cache
 */
typedef struct cache{
	int reads;
	int writes;
	int hits;
	int miss;
} cache;

/**
 * creating a structure called node to hold values of each line
 */
typedef struct line{
	int valid;
	unsigned long long set;
	unsigned long long tag;
}line;


/*
 * fullList structure is just used for a fullassociative list
 */
typedef struct fullList{
	int size;
	line *numLines;
}fullList;

/*
 * creating hashtable sturcutre for direct and nassoc
 */
typedef struct hashTable{
	int size;
	int linesPerSet;
	line** numLines;
}hashTable;

fullList* readOrWriteFullAssoc(unsigned long long retTag, char readOrwrite,fullList *list, cache *cacheA);
fullList* createFullAssocList(fullList *A);
hashTable* createHashTableDN(int linesPerSet, int sets, hashTable *hashInput);
hashTable* hashTableSearch(line *inputLine, hashTable *input,char readOrWrite, cache *inputCache);
int findAssoc(char assoc[]);
int numofSets(unsigned int cachesize, unsigned int blocksize, int assoc);
int findNumLines(int cachesize, int blocksize,int assoc);
void findTagIndex(unsigned int blocksize ,unsigned int cachesize, int numSets,int assoc, int tagIndex[]);
void printOutput(cache *A, cache *B);
unsigned long long callField(unsigned long long memoryAddress, int start, int size);
unsigned long long convertHexToLong(unsigned long long memoryAddress, int tagIndex[]);
void convertHexToLine(unsigned long long memoryAddress,int tagIndex[], line *line1, line *line2);

#endif /* FIRST_H_ */
