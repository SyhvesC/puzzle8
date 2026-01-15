#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "board.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct Node {
	struct Node *parent;
	Board board;
	int heuristic;
	int depth;
	int f_cost;
} Node;

typedef struct {
	Node **elements;
	size_t size;
	size_t capacity;
} PriorityQueue;


Node *create_node(Node *parent, const Board board, const int heuristic, const int depth);

void insert_queue(PriorityQueue *pq, Node *x);
Node *pop_queue(PriorityQueue *pq);
void print_heap_tree(PriorityQueue *pq, size_t index, int depth);

#endif
