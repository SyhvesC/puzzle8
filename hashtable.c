#include "hashtable.h"
#include "board.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define INITIAL_CAPACITY 100000007

static uint32_t generate_hash(const Board *board)
{
	uint64_t x = board->pieces;

	// XorShift
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    x = x ^ (x >> 31);
    return (uint32_t)x;
}

HashTable create_hash_table()
{
	HashTable hash_table = {.size = INITIAL_CAPACITY, .count = 0};
	HashEntry **entries = calloc(hash_table.size, sizeof(HashEntry*));

	if (entries == NULL) {
		fprintf(stderr, "Error, could not allocate memory for the hashtable entries!");
		exit(-1);
	}

	hash_table.entries = entries;
	return hash_table;
}

void insert_hash_entry(HashTable *table, Node* node)
{
	HashEntry *entry = malloc(sizeof(HashEntry));
	if (entry == NULL) {
		fprintf(stderr, "Error, could not allocate memory for new hash entry!");
		exit(-1);
	}

	uint32_t index = generate_hash(&node->board) % table->size;

	entry->node = node;
	entry->next = table->entries[index];
	table->entries[index] = entry;
	table->count++;
	
	if (entry->next != NULL)
		table->collisions++;
}

Node* lookup_hash(HashTable *table, Board *board)
{
	uint32_t index = generate_hash(board) % table->size;
	HashEntry *entry = table->entries[index];

	while (entry != NULL) {
		if (entry->node->board.pieces == board->pieces) {
			return entry->node;
		}
		entry = entry->next;
	}
	return NULL;
}

void destroy_hash_table(HashTable *t)
{
	for (uint32_t i = 0; i < t->size; i++) {
		HashEntry *e = t->entries[i];

		while (e != NULL) {
			if (e->node != NULL)
				free(e->node);

			HashEntry *next = e->next;
			free(e);
			e = next;
		}
	}
	free(t->entries);
	t->size = 0;
	t->count = 0;
}
