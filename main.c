#include "board.h"
#include "pqueue.h"
#include "hashtable.h"
#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

MoveTable *move_table;
Node *winner;

SolveStatus check_game_inversions(const Board *board)
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

	SolveStatus is_solvable;

	if ((board->side % 2) != 0) {
		is_solvable = (n_inversions % 2 == 0) ? SOLVABLE : NOT_SOLVABLE;
	} else {
		size_t zero_row = board->zero_index / board->side;
		printf("Zero index: %lu\n", zero_row);
		is_solvable = ((n_inversions + zero_row) % 2 != 0) ? SOLVABLE : NOT_SOLVABLE;
	}	
	
	return is_solvable;

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

	//printf("M.Distance: %d\n", total_d);
	return total_d;
}

int8_t fetch_target_index(uint8_t zero_index, BoardType side, Direction dir)
{
	switch(dir) {
		case UP:    return (zero_index < side)				   ? -1 : (int8_t)(zero_index - side);
		case DOWN:  return (zero_index >= side * (side - 1))   ? -1 : (int8_t)(zero_index + side);
		case LEFT:  return ((zero_index % side) == 0)		   ? -1 : (int8_t)(zero_index - 1);
		case RIGHT: return ((zero_index % side) == (side - 1)) ? -1 : (int8_t)(zero_index + 1);
		default: return -1;
	}
}

SolveStatus find_next_states(PriorityQueue *queue, HashTable *table)
{
	Node *next = pop_queue(queue);
	if (next->heuristic == 0) {
		//insert_queue(queue, next);
		winner = next;
		return IS_SOLVED;
	}

	Board *state = &(next->board);

	const MoveTable list = move_table[state->zero_index];
	for (uint8_t i = 0; i < list.count; i++) {
		const int8_t target_index = list.moves[i];
		if (target_index == -1)
			continue;

		Board new_state = *state;
		new_state.pieces[state->zero_index] = new_state.pieces[target_index];
		new_state.pieces[target_index] = 0;
		new_state.zero_index = target_index;

		if (!lookup_hash(table, &new_state)) {

			Node *new_node = create_node(next, &new_state, distance(&new_state), next->depth + 1);
			insert_hash_entry(table, new_node);
			insert_queue(queue, new_node);
		}	
	}
	return NOT_SOLVED;
}

void print_solution(Node *node)
{
    if (node == NULL)
		return; 
    
	print_solution(node->parent);

    print_game_status(&node->board);
    printf("Depth: %d, Cost: %d, Total: %d\n\n", node->depth, node->heuristic, node->f_cost);
}

void generate_move_table(const BoardType side)
{
	uint8_t length = side * side;
	for (uint8_t zero_index = 0; zero_index < length; zero_index++) {
		for (uint8_t i = 0; i < 4; i++) {
			int8_t target_index = fetch_target_index(zero_index, side, i);
			if (target_index != -1) {
				move_table[zero_index].moves[move_table[zero_index].count++] = target_index;
			}
		}
	}
}

int main(void)
{
	// Initial_state
	uint8_t starting_state[PUZZLE_8 * PUZZLE_8] = {8, 7, 6, 5, 1, 3, 4, 2, 0};
	Board start_board = create_board(PUZZLE_8, starting_state);

	SolveStatus is_solvable = check_game_inversions(&start_board);
	if (is_solvable == SOLVABLE)
		printf("The starting board is solvable.\n");
	else {
		printf("The starting board is NOT solvable.\n");
		exit(-1);
	}

	print_game_status(&start_board);
	move_table = calloc(start_board.length, sizeof(MoveTable));
	generate_move_table(start_board.side);

	// Start the queue
	PriorityQueue queue = create_queue();

	// Create closed list which is a hash table
	HashTable hash_table = create_hash_table();

	// Create the first node;
	Node *node = create_node(NULL, &start_board, distance(&start_board), 0);
	insert_queue(&queue, node);
	insert_hash_entry(&hash_table, node);

	SolveStatus result = NOT_SOLVED;
	while (result != IS_SOLVED) {
		result = find_next_states(&queue, &hash_table);
		if (hash_table.count > hash_table.size / 1.5)
			break;
		if (result == IS_SOLVED) {
			printf("Hooray!\n");
			break;
		}
	}	

	//print_heap_tree(&queue, 0, 0);
	print_solution(winner);

	free(move_table);
	destroy_queue(&queue);
	destroy_hash_table(&hash_table);

	return 0;
}
