/*
 *  hypercubesets.c
 *  
 *
 *  Created by Nico Van Cleemput on 05/07/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>     /* strcat */


int structureCount = 0;
short endian = LITTLE_ENDIAN; // defines which endian should be used while exporting pregraph code

FILE *sets;

const char *byte_to_binary(int x, int length)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 1<<(length-1); z > 0; z >>= 1){
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

char write_2byte_number(FILE *f, unsigned short n, short writeEndian) {
    if (writeEndian == BIG_ENDIAN) {
        fprintf(f, "%c%c", n / 256, n % 256);
    } else {
        fprintf(f, "%c%c", n % 256, n / 256);
    }
    return (ferror(f) ? 2 : 1);
}

char writeMulticode(FILE *f, int order, int rank, int currentSetSize, int* currentSet) {
    int graphOrder = order + currentSetSize;
	structureCount++;
	
    unsigned short i, j;
    if (structureCount == 1) { //if first graph
        fprintf(f, ">>multi_code %s<<", (endian == LITTLE_ENDIAN ? "le" : "be"));
    }
    if (graphOrder <= UCHAR_MAX) {
        fprintf(f, "%c", (unsigned char) graphOrder);
    } else {
        fprintf(f, "%c", 0);
        /* big graph */
        if (write_2byte_number(f, (unsigned short) graphOrder, endian) == 2) {
            return (2);
        }
    }
	int extraVertex = order;
	int upperbound = currentSetSize ? order : order - 1;
    for (i = 0; i < upperbound; i++) {
        for (j = 0; j < rank; j++){
		    int neighbour = i ^ (1 << j);
			if (neighbour>i){
			    if(graphOrder <= UCHAR_MAX){
				    fprintf(f, "%c", (unsigned char)(neighbour+1));
				} else {
				    if (write_2byte_number(f, neighbour+1, endian) == 2) {
					    return (2);
					}
				}
			}
		}
		if(currentSet[i]){
		    if(graphOrder <= UCHAR_MAX){
				fprintf(f, "%c", (unsigned char)(extraVertex+1));
			} else {
				if (write_2byte_number(f, extraVertex+1, endian) == 2) {
					return (2);
				}
			}
			extraVertex++;
		}
        //closing 0
        if (graphOrder <= UCHAR_MAX) {
            fprintf(f, "%c", 0);
        } else {
            if (write_2byte_number(f, 0, endian) == 2) {
                return (2);
            }
        }
    }
	for (; i<graphOrder - 1; i++){
        //closing 0
        if (graphOrder <= UCHAR_MAX) {
            fprintf(f, "%c", 0);
        } else {
            if (write_2byte_number(f, 0, endian) == 2) {
                return (2);
            }
        }
	}
    return (ferror(f) ? 2 : 1);
}

void buildSet(int currentVertex, int order, int rank, int* currentSet, int currentSetSize){
    if(currentVertex == order){
	    writeMulticode(stdout, order, rank, currentSetSize, currentSet);
		/*fprintf(sets, "%d: ", structureCount);
		int i;
		for(i=0; i<order; i++){
		    if(currentSet[i]) fprintf(sets, "%s ", byte_to_binary(i, rank));
		}
		fprintf(sets, "\n");
		*/
		return;
	}
	//first try the set that doesn't contain the current vertex
	currentSet[currentVertex] = 0;
	buildSet(currentVertex + 1, order, rank, currentSet, currentSetSize);
	
	//only build sets for which the complement is larger
	if(currentSetSize < order/2){
	    //next the set that does contain the current vertex
	    currentSet[currentVertex] = 1;
	    buildSet(currentVertex + 1, order, rank, currentSet, currentSetSize+1);
	}
}

int main(int argc, char** argv) {
    if (argc != 2) {
        return 0;
    }

    //parse the rank
    int rank = strtol(argv[1], NULL, 10);
	int order = 1 << rank;
	int set[order];
	char filename[20];
	
	//sprintf(filename, "smallsets%d", rank);	
	//sets = fopen(filename, "w");
	
	fprintf(stderr, "Number of vertices in hypercube: %d\n", order);
	
	set[0] = 1;
	buildSet(1, order, rank, set, 1);
	
	fprintf(stderr, "Total number of sets: %d\n", structureCount);
	
	//fclose(sets);
}
