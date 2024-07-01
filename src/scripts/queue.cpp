#include "scripts/queue.h"

void queue_init(struct Queue* q, int max_size){
    q->root = NULL;
    q->top = NULL;
    q->max_size = max_size;
}

void queue_push(struct Queue* q, Input input_data){
    if(q->size == q->max_size) {
        queue_pop(q);
        q->size--;
    }
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    new_node->input_data = input_data;
    if(!q->root){
        q->root = new_node;
        q->top = new_node;
    }else{
        q->top->next = new_node;
        q->top = new_node;
    }
    q->size++;
}
   
Input queue_pop(struct Queue* q){
    Input input = {.enemy = {.enemy_location = ENEMY_LOC_NONE, .enemy_range = ENEMY_RANGE_NONE}};
    if(q->root){
        Input res = q->root->input_data;
        struct Node* next_root = q->root->next;
        q->root = next_root;
        q->size--;
        return res;
    }
    return input;
}

Input queue_peek_root(struct Queue* q){
    Input input = {.enemy = {.enemy_location = ENEMY_LOC_NONE, .enemy_range = ENEMY_RANGE_NONE}};
    if(q->root) input = q->root->input_data;
    return input;
}
Input queue_peek_top(struct Queue *q){
    Input input = {.enemy = {.enemy_location=  ENEMY_LOC_NONE, .enemy_range = ENEMY_RANGE_NONE}};
    if(q->top) return q->top->input_data;
    return input;
}

Enemy queue_last_enemy(struct Queue *q){
    Node* curr = q->root;
    Enemy enemy = {.enemy_location=ENEMY_LOC_NONE, .enemy_range=ENEMY_RANGE_NONE};
    while(curr){
        if(enemy_on_left(curr->input_data.enemy) || enemy_on_right(curr->input_data.enemy))
            enemy = curr->input_data.enemy;
        curr = curr->next;
    }
    return enemy;
}
