#include "queue.h"
#include <stdlib.h>

void queue_init(Queue *q)
{
    q->head = q->tail = NULL;
    q->size = 0;
}

int queue_push(Queue *q, int value)
{
    QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
    if (!node)
        return -1;

    node->value = value;
    node->next  = NULL;

    if (q->tail)
        q->tail->next = node;
    else
        q->head = node;

    q->tail = node;
    q->size++;
    return 0;
}

int queue_pop(Queue *q, int *value)
{
    if (!q->head)
        return -1;

    QueueNode *node = q->head;
    if (value)
        *value = node->value;

    q->head = node->next;
    if (!q->head)
        q->tail = NULL;

    free(node);
    q->size--;
    return 0;
}

void queue_free(Queue *q)
{
    QueueNode *cur = q->head;
    while (cur) {
        QueueNode *next = cur->next;
        free(cur);
        cur = next;
    }
    q->head = q->tail = NULL;
    q->size = 0;
}

int *queue_to_array(const Queue *q, size_t *out_size)
{
    if (out_size)
        *out_size = q->size;

    if (q->size == 0)
        return NULL;

    int *arr = (int *)malloc(q->size * sizeof(int));
    if (!arr)
        return NULL;

    QueueNode *node = q->head;
    size_t i = 0;
    while (node) {
        arr[i++] = node->value;
        node = node->next;
    }
    return arr;
}
