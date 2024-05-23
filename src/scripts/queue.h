#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h> // Needed for malloc
#include <stdbool.h>

struct Node {
    int val;
    struct Node *next;
};

struct Queue {
    struct Node* root;
    struct Node* top;
    int size;
    int max_size;
};

void queue_init(struct Queue* q, int max_size);
void queue_push(struct Queue* q, int val);
int queue_pop(struct Queue* q);
int queue_peek_root(struct Queue *q);
int queue_peek_top(struct Queue *q);
bool queue_empty(struct Queue* q);

#endif