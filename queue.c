/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Muhammad Ahmad, Timothy MacDougall, Devin Westbye
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Define your FIFO queue functions here, these will most likely be the
// push and pop functions that you declared in your header file
void push (struct queue* queue, struct process* process) {
	struct node* newNode = malloc(sizeof(struct node));
	newNode->process = process;
	newNode->next = NULL;

	if (queue->head == NULL) {
		queue->head = newNode;
		queue->tail = newNode;
	} else {
		queue->tail->next = newNode;
		queue->tail = newNode;
	}
}

struct process* pop (struct queue* queue) {
	if (queue->head == NULL) {
		return NULL;
	}

	struct node* firstNode = queue->head;
	struct process* process = queue->head->process;
	if (queue->head == queue->tail) {
		queue->head = NULL;
		queue->tail = NULL;
	} else {
		queue->head = queue->head->next;
	}
	free(firstNode);

	return process;
}