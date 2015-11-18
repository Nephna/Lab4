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

// Define your utility functions here, you will likely need to add more...

void initializeResources (struct resources* resources) {
	resources->numPrinters = 0;
	resources->numScanners = 0;
	resources->numModems = 0;
	resources->numCDDrives = 0;
	for (int i = 0; i < 1024; i++)
		resources->memory[i] = 0;
}

struct process* createProcess (int arrivalTime, int priority, int runTime, int memory, 
						int printers, int scanners, int modems, int cdDrives) {

	struct process* process = malloc(sizeof(struct process));
	process->arrivalTime = arrivalTime;
	process->priority = priority;
	process->runTime = runTime;
	process->memory = memory;
	process->printers = printers;
	process->scanners = scanners;
	process->modems = modems;
	process->cdDrives = cdDrives;

	return process;
}

int allocateMemory (struct resources* resources, int memory) {
 	int allocated = 1;

 	for (int i = 0; i < MEMORY; i++) {
 		if (resources->memory[i] == 0) {
 			for (int j = i; j < i+memory; j++) {
 				if (j >= 1024) {
					// exceeded MEMORY, erase changes
 					freeMemory(resources, i, 1024-i);
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

void loadDispatch (char fileName[], struct queue* queue) {
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

				if (args[1][0]-'0' == 0) {
				push(queue, newProcess); // add to primary queue
			}
			else {
				push(queue, newProcess); // add to secondary queue
			}
		}
	} else {
		printf("File could not be opened\nClosing program...\n");
		exit(0);
	}
	fclose(file); // close file
}