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
int  queue_push(Queue *q, int value);   /* 0 – ок, !=0 – ошибка */
int  queue_pop(Queue *q, int *value);   /* 0 – ок, !=0 – пусто  */
void queue_free(Queue *q);
int *queue_to_array(const Queue *q, size_t *out_size);

#endif
