#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct Node {
				struct Node *parent;
				int value;
} Node;

typedef struct {
				Node **elements;
				size_t size;
				size_t capacity;
} PriorityQueue;

Node *create_node(int value)
{
				Node *a = malloc(sizeof(Node));
				a->value = value;
				a->parent = NULL;
				return a;
}

void swap_nodes(PriorityQueue *pq, size_t a, size_t b)
{
				Node *temp = pq->elements[a];
				pq->elements[a] = pq->elements[b];
				pq->elements[b] = temp;
}

void insert_queue(PriorityQueue *pq, Node *x)
{
				size_t index = pq->size;
				pq->elements[index] = x;
				pq->size++;
				printf("Insert node with value: %d\n", x->value);

				while (index > 0) {
								size_t p_index = (index - 1) / 2;
								if (pq->elements[p_index]->value > pq->elements[index]->value) {
												swap_nodes(pq, p_index, index);
												index = p_index;
								}
								else
												break;
				}

}	

void pop_queue(PriorityQueue *pq)
{
				if (!pq->size)
								return;

				Node *temp = pq->elements[0];
				free(temp);

				pq->elements[0] = pq->elements[pq->size - 1];
				pq->size--;

				size_t index = 0;
				
				while (1) {
								size_t smallest = index;
								size_t left = index * 2 + 1;
								size_t right = index * 2 + 2;

								if (left < pq->size && pq->elements[left]->value < pq->elements[smallest]->value)
												smallest = left;

								if (right < pq->size && pq->elements[right]->value < pq->elements[smallest]->value)
												smallest = right;

								if (smallest != index) {
												swap_nodes(pq, index, smallest);
												index = smallest;
								}

								else
												break;
				}				
}

void print_queue(PriorityQueue pq)
{
				for (size_t i = 0; i < pq.size; i++)
								printf("%d -> ", pq.elements[i]->value);
}

int main(void)
{
				PriorityQueue pq = {.size = 0, .capacity = 16};
				Node **heap = malloc(sizeof(Node*) * pq.capacity); 
				pq.elements = heap; 
				Node *a = create_node(5);
				insert_queue(&pq, a);

				a = create_node(10);
				insert_queue(&pq, a);
				insert_queue(&pq, create_node(3));
				insert_queue(&pq, create_node(81));
				insert_queue(&pq, create_node(22));
				insert_queue(&pq, create_node(43));
				insert_queue(&pq, create_node(18));

				print_queue(pq);
				printf("\n\n");

				pop_queue(&pq);
				pop_queue(&pq);

				print_queue(pq);

				for (size_t i = 0; i < pq.size; i++)
								free(pq.elements[i]);
				free(pq.elements);
				return 0;
}
