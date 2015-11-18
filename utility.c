/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Muhammad Ahmad, Timothy MacDougall, Devin Westbye
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"

 #define MEMORY 1024

// Define your utility functions here, you will likely need to add more...

void initializeResources (struct resources* resources) {
	resources->numPrinters = 2;
	resources->numScanners = 1;
	resources->numModems = 1;
	resources->numCDDrives = 2;

	for (int i = 0; i < MEMORY; i++)
		resources->memory[i] = 0;
}

struct process* createProcess (int arrivalTime, int priority, int runTime, int memory, 
						int printers, int scanners, int modems, int cdDrives) {

	struct process* process = malloc(sizeof(struct process));
	process->arrivalTime = arrivalTime;
	process->priority = priority;
	process->runTime = runTime;
	process->pid = 0;
	process->address = -1;
	process->memory = memory;
	process->suspended = 0;
	process->printers = printers;
	process->scanners = scanners;
	process->modems = modems;
	process->cdDrives = cdDrives;

	return process;
}

int allocateMemory (struct resources* resources, int memory, int priority) {
	// local variable declarations
 	int allocated = 1;
 	int maxMemory;

 	if (priority == 0)
 		maxMemory = MEMORY;
 	else
 		maxMemory = MEMORY-64;

 	for (int i = 0; i < maxMemory; i++) {
 		if (resources->memory[i] == 0) {
 			for (int j = i; j < i+memory; j++) {
 				if (j >= maxMemory) {
					// exceeded maxMemory, erase changes
 					freeMemory(resources, i, maxMemory-i);
					return -1; // could not allocate memory
				}

				if (resources->memory[j] == 0) {
					resources->memory[j] = 1;
				}
				else {
					// memory block occupied by other process, erase changes
					freeMemory(resources, i, j-i);
					i += j; // allow i to jump to the point the new block was detected
					allocated = 0;
					break;
				}
			}
			if (allocated == 1)
				return i; // return start of allocated memory block
		}
	}
	return -1; // could not allocate memory
}

void freeMemory (struct resources* resources, int address, int memory) {
	for (int i = address; i < address+memory; i++) {
		resources->memory[i] = 0;
	}
}

int allocateIO (struct resources* resources, int ioRequirements[4]) {
	// get current amount of each resource from the resources list and store
	// them in an array for easy iteration
	int availResources[4] = {resources->numPrinters, resources->numScanners,
							 resources->numModems, resources->numCDDrives};
	// loop 4 times, 1 time for each resource, and compare the two arrays
	for (int i = 0; i < 4; i++) {
		// check if the resources list does not have enough resources available
		// to allocate to the process
		if (ioRequirements[i] > availResources[i]) {
			printf("%d %d\n", ioRequirements[i], availResources[i]);
			return 0; // not enough resources available, return failed allocation
		}
	}
	// enough resources are available to allocate, so allocate them
	resources->numPrinters -= ioRequirements[0];
	resources->numScanners -= ioRequirements[1];
	resources->numModems -= ioRequirements[2];
	resources->numCDDrives -= ioRequirements[3];

	return 1; // successfully allocated resources
}

void freeIO (struct resources* resources, int allocatedIO[4]) {
	// add all allocated IO back into the resources list
	resources->numPrinters += allocatedIO[0];
	resources->numScanners += allocatedIO[1];
	resources->numModems += allocatedIO[2];
	resources->numCDDrives += allocatedIO[3];
}

int allocateResources (struct resources* resources, int reqResources[5], int priority) {
	// attempt to allocate memory for the process
	int address = allocateMemory(resources, reqResources[0], priority);
	// create an array of all IO devices the process requires
	int ioRequirements[4] = {reqResources[1], reqResources[2], reqResources[3], reqResources[4]};
	// attempt to allocate the IO devices
	int allocatedIO = allocateIO(resources, ioRequirements);

	if (address == -1 || allocatedIO == 0) // if memory or IO devices could not be allocated
		return -1; // return failed allocation
	else
		return address; // successfully allocated resources, return address of allocated memory
}

void freeResources (struct resources* resources, int allocatedResources[5], int address) {
	freeMemory(resources, address, allocatedResources[0]); // free allocated memory
	// create array of all IO devices previously allocated for the process
	int allocatedIO[4] = {allocatedResources[1], allocatedResources[2], allocatedResources[3], allocatedResources[4]};
	freeIO(resources, allocatedIO); // free IO devices
}

void loadDispatch (char fileName[], struct queue* realtime, struct queue* priorityOne, struct queue* priorityTwo,
				 																	   struct queue* priorityThree) {
	FILE *file = fopen(fileName, "r");

	if (file != NULL) {
		char line[256];
		char args[8][8];

		while (fgets(line, 256, file) != NULL) {
			char delim[] = ", ";
			char* token = strtok(line, delim);

			int i = 0;
			while (token != NULL) { // loop through the line seperate each word to add to args
				strcpy(args[i], token);
				token = strtok(NULL, delim);
				i++;
			}
			// create a new process from each line of the file
			struct process* newProcess = createProcess (atoi(args[0]), atoi(args[1]), atoi(args[2]), atoi(args[3]), 
												 atoi(args[4]), atoi(args[5]), atoi(args[6]), atoi(args[7]));

			if (atoi(args[1]) == 0)
				push(realtime, newProcess); // add to primary queue
			else if (atoi(args[1]) == 1)
				push(priorityOne, newProcess); // add to secondary queue
			else if (atoi(args[1]) == 2)
				push(priorityTwo, newProcess);
			else
				push(priorityThree, newProcess);
		}
	} else {
		printf("File could not be opened\nClosing program...\n");
		exit(0);
	}
	fclose(file); // close file
}