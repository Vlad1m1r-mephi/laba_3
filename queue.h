#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

typedef struct QueueNode {
    int value;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue {
    QueueNode *head;
    QueueNode *tail;
    size_t size;
} Queue;

void queue_init(Queue *q);
int queue_push(Queue *q, int value);
int queue_pop(Queue *q, int *value);
void queue_free(Queue *q);
void queue_print(const Queue *q);
int queue_edit_at(Queue *q, size_t index, int new_value);
void queue_selection_sort(Queue *q);
void queue_quick_sort(Queue *q);
Queue* queue_copy(const Queue *q);
int queue_is_empty(const Queue *q);

#endif