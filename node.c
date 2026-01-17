#include "node.h"
#include "board.h"

#include <stdio.h>
#include <stdlib.h>

Node *create_node(Node *parent, const Board *board, const int heuristic, const int depth)
{
	Node *node = malloc(sizeof(Node));
	if (!node) {
		fprintf(stderr, "Error, could not allocate memory for the node!");
		exit(-1);
	}

	node->parent = parent;
	node->board = *board;
	node->heuristic = heuristic;
	node->depth = depth;
	node->f_cost = heuristic + depth;

	return node;
}
