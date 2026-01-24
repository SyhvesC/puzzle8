#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

//#include "board.h"
#include "board.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct {
	Node **elements;
	uint64_t size;
	uint64_t capacity;
} PriorityQueue;

PriorityQueue create_queue();
void min_heapify(PriorityQueue *queue, uint64_t index);
void insert_queue(PriorityQueue *queue, Node *node);
Node *pop_queue(PriorityQueue *queue);
void print_heap_tree(PriorityQueue *queue, size_t index, int depth);
void destroy_queue(PriorityQueue *queue);

#endif
