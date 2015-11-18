/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Muhammad Ahmad, Timothy MacDougall, Devin Westbye
 * All rights reserved.
 * 
 */
#ifndef UTILITY_H_
#define UTILITY_H_

 #include "queue.h"

// The amount of available memory
#define MEMORY 1024

// Resources structure containing integers for each resource constraint and an
// array of 1024 for the memory
struct resources {
	int numPrinters;
	int numScanners;
	int numModems;
	int numCDDrives;
	int memory[MEMORY];
};

// Processes structure containing all of the process details parsed from the 
// input file, should also include the memory address (an index) which indicates
// where in the resources memory array its memory was allocated
struct process {
	int arrivalTime;
	int priority;
	int runTime;
	int memory;
	int printers;
	int scanners;
	int modems;
	int cdDrives;
};

// Include your relevant functions declarations here they must start with the 
// extern keyword such as in the following examples:

extern void initializeResources (struct resources* resources);

extern struct process* createProcess (int arrivalTime, int priority, int runTime, int memory, 
							   int printers, int scanners, int modems, int cdDrives);

// Function to allocate a contiguous chunk of memory in your resources structure
// memory array, always make sure you leave the last 64 values (64 MB) free, should
// return the index where the memory was allocated at
// extern int alloc_mem(resources res, int size);
extern int allocateMemory (struct resources* resources, int memory);

// Function to free the allocated contiguous chunk of memory in your resources
// structure memory array, should take the resource struct, start index, and 
// size (amount of memory allocated) as arguments
// extern free_mem(resources res, int index, int size);
extern void freeMemory (struct resources* resources, int address, int memory);

// Function to parse the file and initialize each process structure and add
// it to your job dispatch list queue (linked list)
// extern void load_dispatch(char *dispatch_file, node_t *queue);
extern void loadDispatch (char fileName[], struct queue* queue);


#endif /* UTILITY_H_ */