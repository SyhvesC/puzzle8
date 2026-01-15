#include "board.h"
#include "pqueue.h"

#include <stdio.h>

#define MAX_PRINT_DEPTH 100
#define RESIZE_FACTOR 2

Node *create_node(Node *parent, const Board board, const int heuristic, const int depth)
{
	Node *node = malloc(sizeof(Node));
	if (!node) {
		fprintf(stderr, "Error, could not allocate memory for the node!");
		exit(-1);
	}

	node->parent = parent;
	node->board = board;
	node->heuristic = heuristic;
	node->depth = depth;
	node->f_cost = heuristic + depth;

	return node;
}

static void swap_nodes(PriorityQueue *pq, const size_t a, const size_t b)
{
	Node *temp = pq->elements[a];
	pq->elements[a] = pq->elements[b];
	pq->elements[b] = temp;
}

static void resize_queue(PriorityQueue *pq)
{
	size_t new_capacity = pq->capacity * RESIZE_FACTOR;
	Node **temp_h = realloc(pq->elements, new_capacity * sizeof(Node*));
	if (!temp_h) {
		fprintf(stderr, "Error, could not reallocate new memory for the heap!\n");
		exit(-1);
	}

	pq->elements = temp_h;
	pq->capacity = new_capacity;

	printf("New heap capacity extended to %lu.\n", pq->capacity);
}

void insert_queue(PriorityQueue *pq, Node *x)
{
	if (pq->size == pq->capacity) {
		printf("Queue has reached maximum capicity: %lu / %lu!\n", pq->size, pq->capacity);
		resize_queue(pq);
	}
	size_t index = pq->size;
	pq->elements[index] = x;
	pq->size++;
	printf("Insert node with value: %d\n", x->heuristic);

	while (index > 0) {
		size_t p_index = (index - 1) / 2;
		if (pq->elements[p_index]->heuristic > pq->elements[index]->heuristic) {
			swap_nodes(pq, p_index, index);
			index = p_index;
		}
		else
		break;
	}

}	

Node* pop_queue(PriorityQueue *pq)
{
	if (!pq->size)
		return NULL;

	Node *temp = pq->elements[0];

	pq->elements[0] = pq->elements[pq->size - 1];
	pq->size--;

	size_t index = 0;

	while (1) {
		size_t smallest = index;
		size_t left = index * 2 + 1;
		size_t right = index * 2 + 2;

		if (left < pq->size && pq->elements[left]->heuristic < pq->elements[smallest]->heuristic)
			smallest = left;

		if (right < pq->size && pq->elements[right]->heuristic < pq->elements[smallest]->heuristic)
			smallest = right;

		if (smallest != index) {
			swap_nodes(pq, index, smallest);
			index = smallest;
		}

		else
		break;
	}

	return temp;
}

void print_heap_tree(PriorityQueue *pq, size_t index, int depth)
{
	if (index >= pq->size || depth > MAX_PRINT_DEPTH)
		return;

	print_heap_tree(pq, 2 * index + 2, depth + 1);

	for (int i = 0; i < depth; i++)
		printf("    ");
	printf("|-- %d\n", pq->elements[index]->heuristic);
	//for (uint8_t i = 0; i < pq->elements[index]->board.length; i++)
	//printf("-%d", pq->elements[index]->board.pieces[i]);
	print_heap_tree(pq, 2 * index + 1, depth + 1);
}


void print_queue(PriorityQueue pq)
{
	for (size_t i = 0; i < pq.size; i++)
		printf("%d -> ", pq.elements[i]->heuristic);
}
