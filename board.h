#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

typedef enum {
	PUZZLE_8 = 3,
	PUZZLE_15 = 4
} BoardType;

typedef struct {
	uint64_t pieces;
	BoardType side;
	uint8_t zero_index;
} Board;

typedef struct Node {
	struct Node *parent;
	int64_t heap_index;
	uint8_t heuristic;
	uint8_t depth;
	uint8_t f_cost;
	Board board;
} Node;

typedef enum {
	NOT_SOLVABLE,
	SOLVABLE,
	IS_SOLVED,
	NOT_SOLVED,
} SolveStatus;

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT
} Direction;

typedef struct {
	uint8_t moves[4];
	uint8_t count;
} MoveTable;

#endif
