/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Muhammad Ahmad, Timothy MacDougall, Devin Westbye
 * All rights reserved.
 * 
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "queue.h"
#include "utility.h"
#include "hostd.h"

// Put macros or constants here using #define
#define MEMORY 1024
#define DISPATCHLIST "dispatchlist"
#define PROCESS "./process"

// Put global environment variables here

 void executeQueue (struct resources* resources, struct queue* queue, struct queue* nextQueue) {
    // variables internal to executeQueue
    struct process* process;
    pid_t pid = 0;
    process = (struct process*)malloc(sizeof(struct process));

    // loop through all processes in the queue
    while (queue->head != NULL) {
        process = pop(queue); // get a process from the queue
        int reqResources[5];

        if (process->address == -1) { // if process has unallocated resources, try to allocate them
            // create an array of all resources the process requires
            reqResources[0] = process->memory;
            reqResources[1] = process->printers;
            reqResources[2] = process->scanners;
            reqResources[3] = process->modems;
            reqResources[4] = process->cdDrives;

            process->address = allocateResources(resources, reqResources, process->priority);

            if (process->address == -1) {
                // not enough memory available, push back onto the queue
                push(queue, process);
                continue;
            }
        }

        if (process->pid == 0) {
            pid = fork(); // create a new child process for every process in the queue that has not ran
            if (pid != 0)
                process->pid = pid; // set process pid in parent
        } else {
            pid = process->pid; // get pid of currently running child
        }
        // child process
        if (pid == 0) {
            process->pid = getpid(); // set process pid in child
            execl(PROCESS, 0);
            exit(0);
        }
        // parent process
        if (process->suspended == 1) {
            kill(pid, SIGCONT);
        }
        if (process->priority == 0) { // if process is in realTime queue
            sleep(process->runTime); // sleep for full duration of process
            kill(pid, SIGINT); // terminate process after runTime
            waitpid(pid, NULL, 0); // join child process
            freeResources(resources, reqResources, process->address); // free resources
        } else {
            sleep(1); // wait for the process to run for 1 second
            process->runTime -= 1;
            if (process->runTime == 0) {
                kill(pid, SIGINT); // terminate process
                waitpid(pid, NULL, 0); // join process
                freeResources(resources, reqResources, process->address); // free resources
            } else {
                kill(pid, SIGTSTP); // pause process
                process->suspended = 1;
                push(nextQueue, process);
            }
        }
        process = NULL;
    }
 }

int main(int argc, char *argv[]) {
    // variable declaration
    struct queue *realtime, *priorityOne, *priorityTwo, *priorityThree;
    struct resources* resources;

    // variable initialization
    realtime = (struct queue*)malloc(sizeof(struct queue));
    priorityOne = (struct queue*)malloc(sizeof(struct queue));
    priorityTwo = (struct queue*)malloc(sizeof(struct queue));
    priorityThree = (struct queue*)malloc(sizeof(struct queue));
    resources = (struct resources*)malloc(sizeof(struct resources));
    initializeResources(resources);

    // load the dispatchlist and add process structure instance to 
    // job dispatch list queue
    
    loadDispatch(DISPATCHLIST, realtime, priorityOne, priorityTwo, priorityThree);

    // execute all processes in the realtime queue completely
    //executeQueue(resources, realtime, NULL);

    // execute processes in the priorityOne queue
    executeQueue(resources, priorityOne, priorityTwo);

    // Iterate through each item in the job dispatch list, add each process
    // to the appropriate queues

    // Allocate the resources for each process before it's executed

    // Execute the process binary using fork and exec

    // Perform the appropriate signal handling / resource allocation and de-alloaction

    // Repeat until all processes have been executed, all queues are empty

    free(realtime), free(priorityOne), free(priorityTwo), free(priorityThree);
    free(resources);
     
    return EXIT_SUCCESS;
}
