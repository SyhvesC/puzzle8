#ifndef NODE_H
#define NODE_H

#include "board.h"

typedef struct Node {
	struct Node *parent;
	Board board;
	int heuristic;
	int depth;
	int f_cost;
} Node;

Node *create_node(Node *parent, const Board *board, const int heuristic, const int depth);

#endif
