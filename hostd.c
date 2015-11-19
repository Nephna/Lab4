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
int dispatchTime;
int status;

 int executeQueue (struct resources* resources, struct queue* queue, struct queue* nextQueue) {
    // variables internal to executeQueue
    struct process* process;
    struct process* tracker;
    pid_t pid = 0;
    process = (struct process*)malloc(sizeof(struct process));
    tracker = NULL;

    // loop through all processes in the queue
    while (queue->head != NULL) {
        process = pop(queue); // get a process from the queue
        int reqResources[5];

        if (tracker == process) {
            push(queue, process);
            return 0; // no processes to execute, exit with failure
        }
        if (tracker == NULL)
            tracker = process;

        // create an array of all resources the process requires
        reqResources[0] = process->memory;
        reqResources[1] = process->printers;
        reqResources[2] = process->scanners;
        reqResources[3] = process->modems;
        reqResources[4] = process->cdDrives;

        if (process->address == -1) { // if process has unallocated resources, try to allocate them
            if (process->arrivalTime > dispatchTime) {
                push(queue, process);
                return 0; // process has not arrived, return with failure
            }

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
            //printf("PID: %d    Arrival Time: %d     Priority: %d    Runtime: %d     Memory: %d\nPrinters: %d    Scanners: %d    Modems: %d  Cd Drives: %d\n",
            //        process->pid, process->arrivalTime, process->priority, process->runTime, process->memory,
            //        process->printers, process->scanners, process->modems, process->cdDrives);
            printf("Required resources: Printers: %d Scanners: %d Modems: %d Cd Drives: %d\n",
            process->printers, process->scanners, process->modems, process->cdDrives);
            printf("Current resources:  Printers: %d Scanners: %d Modems: %d Cd Drives: %d\n", 
            resources->numPrinters, resources->numScanners, resources->numModems, resources->numCDDrives);
            execl(PROCESS, 0);
            exit(0);
        }
        // parent process
        if (process->suspended == 1)
            kill(pid, SIGCONT);

        if (process->priority == 0) { // if process is in realTime queue
            sleep(process->runTime); // sleep for full duration of process
            kill(pid, SIGINT); // terminate process after runTime
            waitpid(pid, &status, WUNTRACED); // join child process
            freeResources(resources, reqResources, process->address); // free resources
        } else {
            sleep(1); // wait for the process to run for 1 second
            process->runTime -= 1;
            if (process->runTime == 0) {
                kill(pid, SIGINT); // terminate process
                waitpid(pid, &status, WUNTRACED); // join process
                freeResources(resources, reqResources, process->address); // free resources
            } else {
                kill(pid, SIGTSTP); // pause process
                waitpid(pid, &status, WUNTRACED);
                process->suspended = 1;
                push(nextQueue, process);
            }
        }
        process = NULL;
        tracker = NULL;
        return 1; // process executed, return with success
    }
    process = NULL;
    tracker = NULL;
    return 0; // queue is empty, return failure
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

    dispatchTime = 0;

    // load the dispatchlist and add process structure instance to 
    // job dispatch list queue
    
    loadDispatch(DISPATCHLIST, realtime, priorityOne, priorityTwo, priorityThree);

    // execute all processes in the realtime queue completely
    //executeQueue(resources, realtime, NULL);

    // execute processes in the priorityOne queue
    while (realtime->head != NULL || priorityOne->head != NULL || priorityTwo->head != NULL || priorityThree->head != NULL) {
        if (executeQueue(resources, realtime, NULL) == 1){
            dispatchTime++;
            continue;
        } else if (executeQueue(resources, priorityOne, priorityTwo) == 1) {
            dispatchTime++;
            continue;
        } else if (executeQueue(resources, priorityTwo, priorityThree) == 1) {
            dispatchTime++;
            continue;
        } else if (executeQueue(resources, priorityThree, priorityThree) == 1) {
            dispatchTime++;
            continue;
        }
        dispatchTime++;
    }

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
