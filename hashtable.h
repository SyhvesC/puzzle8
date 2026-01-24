#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "board.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct HashEntry {
	Node *node;
	struct HashEntry *next;
} HashEntry;

typedef struct {
	HashEntry **entries;
	uint32_t size;
	uint32_t count;
	uint32_t collisions;
} HashTable;

HashTable create_hash_table();
void insert_hash_entry(HashTable *table, Node* node);
Node* lookup_hash(HashTable *table, Board *board);
void destroy_hash_table(HashTable *t);

#endif
