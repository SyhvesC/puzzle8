#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef enum {
				PUZZLE_8 = 3,
				PUZZLE_15 = 4
} GameType;

typedef enum {
				NOT_SOLVABLE,
				SOLVABLE,
				IS_SOLVED
} SolveStatus;

const uint8_t GOAL_8[PUZZLE_8 * PUZZLE_8] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
const uint8_t GOAL_15[PUZZLE_15 * PUZZLE_15] = {0, 1, 2 ,3 ,4 , 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

typedef struct {
				uint8_t *pieces;
				uint8_t length;
				uint8_t zero_index;
				GameType side;
				SolveStatus status;
} GameData;

void is_state_solved(GameData *game)
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

void check_game_inversions(GameData *game)
{
				size_t n_inversions = 0;
				for (uint8_t i = 0; i < game->length - 1; i++) {
								if (game->pieces[i] == 0)
												continue;
								for (uint8_t j = i + 1; j < game->length; j++) {
												if (game->pieces[j] == 0)
																continue;
												if (game->pieces[i] > game->pieces[j])
																n_inversions++;
								}
				}
				printf("N of inversions: %lu\n", n_inversions);
				game->status = (n_inversions % 2 == 0) ? SOLVABLE : NOT_SOLVABLE;
}

GameData create_game(const GameType type, const uint8_t *starting_state)
{
				GameData g = {.side = type, .length = type * type};
				g.pieces = (uint8_t*) malloc(g.length);

				if (!g.pieces)
				{
								fprintf(stderr, "Error, could not allocate memory for the puzzle pieces!\n");
								exit(-1);
				}

				for (uint8_t i = 0; i < g.length; i++) {
								g.pieces[i] = starting_state[i];
								if (starting_state[i] == 0)
												g.zero_index = i;
				}
				
				is_state_solved(&g);
				if (g.status == IS_SOLVED)
								return g;
				
				check_game_inversions(&g);

				if (g.status == SOLVABLE)
								printf("The puzzzle is solvable.\n");
				else
								printf("The puzzle is not solvable.\n");
				
				return g;
}

void print_game_status(GameData game)
{
				printf("Current board:\n");
				
				uint8_t newline = 0;
				for (uint8_t i = 0; i < game.length; i++) {
								printf("%d ", game.pieces[i]);
								if (newline++ == game.side - 1) {
												printf("\n");
												newline = 0;
								}	
				}
				printf("\n");
}

int main(void)
{
				uint8_t starting_state[PUZZLE_8 * PUZZLE_8] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
				GameData g = create_game(PUZZLE_8, starting_state);
				
				print_game_status(g);

				free(g.pieces);

				return 0;
}
