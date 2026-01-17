#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

typedef enum {
	PUZZLE_8 = 3,
	PUZZLE_15 = 4
} BoardType;

typedef struct {
	uint8_t pieces[16];
	uint8_t length;
	uint8_t zero_index;
	BoardType side;
} Board;

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
