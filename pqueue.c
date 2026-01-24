//#include "board.h"
#include "pqueue.h"
#include "board.h"

#include <stdio.h>

#define MAX_PRINT_DEPTH 5
#define RESIZE_FACTOR 2
#define INITIAL_CAPACITY 512

static void swap_nodes(PriorityQueue *queue, const size_t a, const size_t b)
{
	Node *temp = queue->elements[a];
	temp->heap_index = a;
	queue->elements[a] = queue->elements[b];
	queue->elements[a]->heap_index = b;
	queue->elements[b] = temp;


}

static void resize_queue(PriorityQueue *queue)
{
	uint64_t new_capacity = queue->capacity * RESIZE_FACTOR;
	Node **temp_h = realloc(queue->elements, new_capacity * sizeof(Node*));
	if (!temp_h) {
		fprintf(stderr, "Error, could not reallocate new memory for the heap!\n");
		exit(-1);
	}

	queue->elements = temp_h;
	queue->capacity = new_capacity;

	//printf("New heap capacity extended to %lu.\n", queue->capacity);
}

PriorityQueue create_queue()
{
	PriorityQueue queue = {.size = 0, .capacity = INITIAL_CAPACITY};
	Node **heap = malloc(sizeof(Node*) * queue.capacity);
	if (!heap) {
		fprintf(stderr, "Error, could not allocate memory for the heap.");
		exit(-1);
	}
	queue.elements = heap;

	return queue;
}

void min_heapify(PriorityQueue *queue, uint64_t index)
{
	while (index > 0) {
		size_t p_index = (index - 1) / 2;
		if (queue->elements[p_index]->f_cost > queue->elements[index]->f_cost) {
			swap_nodes(queue, p_index, index);
			index = p_index;
		}
		else
			break;
	}
}

void insert_queue(PriorityQueue *queue, Node *node)
{
	if (queue->size == queue->capacity) {
		//printf("Queue has reached maximum capicity: %lu / %lu!\n", queue->size, queue->capacity);
		resize_queue(queue);
	}
	uint64_t index = queue->size;
	queue->elements[index] = node;
	node->heap_index = index;
	queue->size++;
	min_heapify(queue, index);
}	

Node* pop_queue(PriorityQueue *queue)
{
	if (!queue->size)
		return NULL;

	Node *temp = queue->elements[0];
	if (temp == NULL) {
		fprintf(stderr, "Error, could not pop node from queue!\n");
		exit(-1);
	}

	queue->elements[0] = queue->elements[queue->size - 1];
	queue->size--;

	uint64_t index = 0;

	while (1) {
		uint64_t smallest = index;
		uint64_t left = index * 2 + 1;
		uint64_t right = index * 2 + 2;

		if (left < queue->size && queue->elements[left]->f_cost < queue->elements[smallest]->f_cost)
			smallest = left;

		if (right < queue->size && queue->elements[right]->f_cost < queue->elements[smallest]->f_cost)
			smallest = right;

		if (smallest != index) {
			swap_nodes(queue, index, smallest);
			index = smallest;
		}

		else
			break;
	}

	return temp;
}

/*void print_heap_tree(PriorityQueue *queue, size_t index, int depth)
{
	if (index >= queue->size || depth > MAX_PRINT_DEPTH)
		return;

	print_heap_tree(queue, 2 * index + 2, depth + 1);

	for (int i = 0; i < depth; i++)
		printf("    ");
	printf("|-- %d  ", queue->elements[index]->heuristic);

	// FOR DEBUGGING
	// #############
	for (uint8_t i = 0; i < queue->elements[index]->board.length; i++)
		printf("-%d", queue->elements[index]->board.pieces[i]);
	printf(" - idx: %d", queue->elements[index]->board.zero_index);
	// #############
	//
	printf("\n");
	print_heap_tree(queue, 2 * index + 1, depth + 1);
}*/

void destroy_queue(PriorityQueue *queue)
{
	free(queue->elements);
	queue->size = 0;
	queue->capacity = 0;
}


