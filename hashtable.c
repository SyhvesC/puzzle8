#include "hashtable.h"
#include "board.h"
#include "node.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define INITIAL_CAPACITY 1000003

static uint32_t generate_hash(const Board *board)
{
	uint32_t hash = 5381;

    for (uint8_t i = 0; i < board->length; i++) {
        hash = ((hash << 5) + hash) + board->pieces[i];
    }

    return hash;
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

bool lookup_hash(HashTable *table, Board *board)
{
	uint32_t index = generate_hash(board) % table->size;
	HashEntry *entry = table->entries[index];

	while (entry != NULL) {
		if (memcmp(entry->node->board.pieces, board->pieces, board->length) == 0) {
			return true;
		}
		entry = entry->next;
	}
	return false;
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
