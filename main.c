#include "board.h"
#include "pqueue.h"
#include "hashtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

MoveTable *move_table;
Node *winner;

#define MAX_TILE_VALUE PUZZLE_15 * PUZZLE_15

uint8_t get_tile(uint64_t board, uint8_t index) {
	if (index >= MAX_TILE_VALUE) {
		fprintf(stderr, "Error, tile index exceeds the permitted range!");
		exit(-1);
	}

    return (uint8_t)(board >> (index * 4)) & 0xF;
}

void set_tile(uint64_t *board, uint8_t index, uint8_t value) {
	if (value >= MAX_TILE_VALUE) {
		fprintf(stderr, "Error, inserting tile value of \"%d\" exceeds the permitted range!", value);
		exit(-1);
	}

    uint64_t mask = (uint64_t)0xF << (index * 4);
    *board = (*board & ~mask) | ((uint64_t)value << (index * 4));
}

void unpack_board(uint64_t board, uint8_t *pieces, uint8_t length)
{
	if (length > MAX_TILE_VALUE) {
		fprintf(stderr, "Error, length of unboard to unpack exceeds maximum range!");
		exit(-1);
	}

	for (uint8_t i = 0; i < length; i++)
		pieces[i] = (uint8_t)(board >> (4 * i)) & 0xF;
}


SolveStatus check_game_inversions(const Board *board)
{
	uint8_t pieces[16];
	uint8_t length = board->side * board->side;
	unpack_board(board->pieces, pieces, length);

	size_t n_inversions = 0;
	for (uint8_t i = 0; i < length - 1; i++) {
		if (pieces[i] == 0)
			continue;
		for (uint8_t j = i + 1; j < length; j++) {
			if (pieces[j] == 0)
				continue;
			if (pieces[i] > pieces[j])
				n_inversions++;
		}
	}


	SolveStatus is_solvable;

	if ((board->side % 2) != 0) {
		is_solvable = (n_inversions % 2 == 0) ? SOLVABLE : NOT_SOLVABLE;
	} else {
		size_t zero_row = board->side - (board->zero_index / board->side);
		is_solvable = ((n_inversions + zero_row) % 2 != 0) ? SOLVABLE : NOT_SOLVABLE;
	}	
	
	return is_solvable;

}

Board create_board(const BoardType type, const uint8_t *state)
{
	Board board = {.side = type};

	for (uint8_t i = 0; i < type * type; i++) {
		set_tile(&board.pieces, i, state[i]);
		if (state[i] == 0)
			board.zero_index = i;
	}

	return board;
}

void print_game_status(const Board *board)
{
	printf("--------------------------\n");
	printf("Current board:\n");

	uint8_t pieces[16];
	uint8_t length = board->side * board->side;
	unpack_board(board->pieces, pieces, length);
	
	uint8_t newline = 0;
	for (uint8_t i = 0; i < length; i++) {
		printf("%3d ", pieces[i]);
		if (newline++ == board->side - 1) {
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

uint16_t distance(const Board *board)
{
	uint8_t pieces[16];
	uint8_t length = board->side * board->side;
	unpack_board(board->pieces, pieces, length);

	uint16_t total_d = 0;
	for (uint8_t i = 0; i < length; i++) {
		if (pieces[i] != i + 1 && pieces[i] != 0) {
			int x1 = i / board->side;
			int y1 = i % board->side;

			int x2 = (pieces[i] - 1) / board->side;
			int y2 = (pieces[i] - 1) % board->side;

			int d = (myabs(x1 - x2) + myabs(y1 - y2));
			total_d += d;
		}
	}
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
	node->heap_index = -2;

	return node;
}

bool solve(uint8_t *starting_state)
{
	Board start_board = create_board(PUZZLE_8, starting_state);

	SolveStatus is_solvable = check_game_inversions(&start_board);

	if (is_solvable == SOLVABLE) {
		printf("The following starting board is solvable:\n");
		print_game_status(&start_board);
	} else {
		printf("The following starting board is NOT solvable.\n");
		return false;
	}
	// Generates the possible moves for the given puzzle size
	move_table = calloc(start_board.side * start_board.side, sizeof(MoveTable));
	generate_move_table(start_board.side);

	// Creates open list which is a priority queue
	PriorityQueue queue = create_queue();
	// Create closed list which is a hash table
	HashTable table = create_hash_table();

	// Create the first node;
	Node *node = create_node(NULL, &start_board, distance(&start_board), 0);
	insert_queue(&queue, node);
	insert_hash_entry(&table, node);

	printf("Exploring nodes...\n");
	while (1) {
		Node *next = pop_queue(&queue);
		if (next->heuristic == 0) {
			winner = next;
			break;
		}

		next->heap_index = -1;

		Board *state = &(next->board);
		const MoveTable list = move_table[state->zero_index];

		for (uint8_t i = 0; i < list.count; i++) {
			const int8_t target_index = list.moves[i];

			if (target_index == -1)
				continue;

			Board new_state = *state;
			uint64_t tile = get_tile(new_state.pieces, target_index);
			set_tile(&new_state.pieces, state->zero_index, tile);
			set_tile(&new_state.pieces, target_index, 0);

			new_state.zero_index = target_index;

			Node *existing = lookup_hash(&table, &new_state);

			if (existing == NULL) {
				Node *new_node = create_node(next, &new_state, distance(&new_state), next->depth + 1);
				insert_hash_entry(&table, new_node);
				insert_queue(&queue, new_node);
			}
			else if (next->depth + 1 < existing->depth) {
				existing->depth = next->depth + 1;
				existing->parent = next;
				existing->f_cost = existing->depth + existing->heuristic;
				if (existing->heap_index >= 0)
					min_heapify(&queue, existing->heap_index);
			}
		}

	}

	printf("A solution has been found!\n\n");
	print_solution(winner);
	printf("Queue size: %ld HashTable size: %d", queue.size, table.count);

	free(move_table);
	destroy_queue(&queue);
	destroy_hash_table(&table);

	return true;


}

int main(void)
{
	uint8_t starting_state[9] = {1, 2, 7, 8, 0, 3, 5, 6, 4};
	solve(starting_state);	
	return 0;
}
