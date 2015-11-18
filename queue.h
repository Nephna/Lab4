/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Muhammad Ahmad, Timothy MacDougall, Devin Westbye
 * All rights reserved.
 * 
 */
#ifndef QUEUE_H_
#define QUEUE_H_

#include "utility.h"

// Your linked list structure for your queue
struct node {
	struct process* process;
	struct node* next;
};

struct queue {
	struct node* head;
	struct node* tail;
};

// Add a new process to the queue, returns the address of the next node added
// to the linked list
extern void push (struct queue* queue, struct process* process);

// remove and return the first process in the list
extern struct process* pop (struct queue* queue);

#endif /* QUEUE_H_ */