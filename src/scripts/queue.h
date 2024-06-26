#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h> // Needed for malloc
#include <stdbool.h>
#include "hal/enemy.h"
#include "hal/line_sensors.h"

typedef struct{
    Enemy enemy;
    Line_Pos line;
} Input;

struct Node {
    Input input_data;
    struct Node *next;
};

struct Queue {
    struct Node* root;
    struct Node* top;
    int size;
    int max_size;
};

void queue_init(struct Queue* q, int max_size);
void queue_push(struct Queue* q, Input input_data);
Input queue_pop(struct Queue* q);
Input queue_peek_root(struct Queue *q);
Input queue_peek_top(struct Queue *q);
bool queue_empty(struct Queue* q);

#endif