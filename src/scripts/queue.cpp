#include "scripts/queue.h"

void queue_init(struct Queue* q, int max_size){
    q->root = NULL;
    q->top = NULL;
    q->max_size = max_size;
}

void queue_push(struct Queue* q, int val){
    if(q->size == q->max_size) {
        queue_pop(q);
        q->size--;
    }
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    new_node->val = val;
    if(!q->root){
        q->root = new_node;
        q->top = new_node;
    }else{
        q->top->next = new_node;
        q->top = new_node;
    }
    q->size++;
}

int queue_pop(struct Queue* q){
    if(q->root){
        int res = q->root->val;
        struct Node* next_root = q->root->next;
        q->root = next_root;
        q->size--;
        return res;
    }
    return -1;
}

int queue_peek_root(struct Queue* q){
    if(q->root) return q->root->val;
    return -1;
}
int queue_peek_top(struct Queue *q){
    if(q->top) return q->top->val;
    return -1;
}

bool queue_empty(struct Queue* q){
    return q->root == NULL;
}