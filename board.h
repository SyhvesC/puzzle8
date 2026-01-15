#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

typedef enum {
				PUZZLE_8 = 3,
				PUZZLE_15 = 4
} BoardType;

typedef enum {
				NOT_SOLVABLE,
				SOLVABLE,
				IS_SOLVED
} SolveStatus;


typedef struct {
				uint8_t pieces[16];
				uint8_t length;
				uint8_t zero_index;
				BoardType side;
				SolveStatus status;
} Board;

#endif
