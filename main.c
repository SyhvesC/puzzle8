#include "board.h"
#include "pqueue.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


const uint8_t GOAL_8[PUZZLE_8 * PUZZLE_8] = {1, 2, 3, 4, 5, 6, 7, 8};
const uint8_t GOAL_15[PUZZLE_15 * PUZZLE_15] = {1, 2 ,3 ,4 , 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};


void is_state_solved(Board *game)
{
	switch(game->side)	{
		case PUZZLE_8:
			if (memcmp(game->pieces, GOAL_8, sizeof(uint8_t) * game->length) == 0)
				game->status = IS_SOLVED;
			break;

		case PUZZLE_15:
			if (memcmp(game->pieces, GOAL_15, sizeof(uint8_t) * game->length) == 0)
				game->status = IS_SOLVED;
			break;

		default:
			fprintf(stderr, "Error, puzzle game with unknown 'side' value!");

	}
}

SolveStatus check_game_inversions(Board *board)
{
	size_t n_inversions = 0;
	for (uint8_t i = 0; i < board->length - 1; i++) {
		if (board->pieces[i] == 0)
			continue;
		for (uint8_t j = i + 1; j < board->length; j++) {
			if (board->pieces[j] == 0)
				continue;
			if (board->pieces[i] > board->pieces[j])
				n_inversions++;
		}
	}
	printf("N of inversions: %lu\n", n_inversions);
	return (n_inversions % 2 == 0) ? SOLVABLE : NOT_SOLVABLE;
}

Board create_board(const BoardType type, const uint8_t *state)
{
	Board board = {.side = type, .length = type * type};

	for (uint8_t i = 0; i < board.length; i++) {
		board.pieces[i] = state[i];
		if (state[i] == 0)
			board.zero_index = i;
	}

	return board;
}

void print_game_status(const Board *game)
{
	printf("Current board:\n");

	uint8_t newline = 0;
	for (uint8_t i = 0; i < game->length; i++) {
		printf("%d ", game->pieces[i]);
		if (newline++ == game->side - 1) {
			printf("\n");
			newline = 0;
		}	
	}
	printf("\n");
}

static uint8_t myabs(int x)
{
	return (x < 0) ? x*-1 : x;
}

int distance(const Board *game)
{
	uint16_t total_d = 0;
	for (uint8_t i = 0; i < game->length; i++) {
		if (game->pieces[i] != i + 1 && game->pieces[i] != 0) {
			//printf("Piece %d is out of place at index %d\n", game->pieces[i], i);
			int x1 = i / game->side;
			int y1 = i % game->side;

			int x2 = (game->pieces[i] - 1) / game->side;
			int y2 = (game->pieces[i] - 1) % game->side;

			int d = (myabs(x1 - x2) + myabs(y1 - y2));
			total_d += d;
			//printf("X1: %d Y1: %d    |     %d, %d   DISTANCE: %d\n\n", x1, y1, x2, y2, d);
		}
	}

	printf("M.Distance: %d\n", total_d);
	return total_d;
}

int main(void)
{
	// Initial_state
	uint8_t starting_state[PUZZLE_8 * PUZZLE_8] = {1, 8, 2, 3, 4, 5, 6, 7, 0};
	Board start_board = create_board(PUZZLE_8, starting_state);

	SolveStatus is_solvable = check_game_inversions(&start_board);
	if (is_solvable == SOLVABLE)
		printf("The starting board is solvable.\n");
	else
		printf("The starting board is NOT solvable.\n");

	print_game_status(&start_board);

	// Start the queue
	PriorityQueue queue = {.size = 0, .capacity = 16};
	Node **heap = malloc(sizeof(Node*) * queue.capacity);
	if (!heap) {
		fprintf(stderr, "Error, could not allocate memory for the heap.");
		exit(-1);
	}
	queue.elements = heap;

	// Create the first node;
	Node *node = create_node(NULL, start_board, distance(&start_board), 0);
	insert_queue(&queue, node);
	print_heap_tree(&queue, 0, 0);

	free(node);
	free(queue.elements);
	return 0;
}
