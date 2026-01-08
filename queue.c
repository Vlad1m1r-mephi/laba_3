#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    node->next = NULL;

    if (q->tail) {
        q->tail->next = node;
    } else {
        q->head = node;
    }
    
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

void queue_print(const Queue *q)
{
    if (!q->head) {
        printf("Очередь пуста\n");
        return;
    }

    QueueNode *node = q->head;
    while (node) {
        printf("%d", node->value);
        node = node->next;
        if (node)
            printf(" ");
    }
    printf("\n");
}

int queue_edit_at(Queue *q, size_t index, int new_value)
{
    if (index >= q->size)
        return -1;

    QueueNode *node = q->head;
    for (size_t i = 0; i < index; i++) {
        node = node->next;
    }
    node->value = new_value;
    return 0;
}

int queue_is_empty(const Queue *q)
{
    return q->head == NULL;
}

void queue_selection_sort(Queue *q)
{
    if (!q->head || !q->head->next)
        return;

    QueueNode *current = q->head;
    
    while (current) {
        QueueNode *min = current;
        QueueNode *runner = current->next;
        
        while (runner) {
            if (runner->value < min->value) {
                min = runner;
            }
            runner = runner->next;
        }
        
        if (min != current) {
            int temp = current->value;
            current->value = min->value;
            min->value = temp;
        }
        
        current = current->next;
    }
}

QueueNode* partition(QueueNode *head, QueueNode *tail, QueueNode **new_head, QueueNode **new_tail)
{
    QueueNode *pivot = tail;
    QueueNode *prev = NULL, *cur = head, *end = pivot;
    
    while (cur != pivot) {
        if (cur->value < pivot->value) {
            if (*new_head == NULL)
                *new_head = cur;
            prev = cur;
            cur = cur->next;
        } else {
            if (prev)
                prev->next = cur->next;
            QueueNode *tmp = cur->next;
            cur->next = NULL;
            end->next = cur;
            end = cur;
            cur = tmp;
        }
    }
    
    if (*new_head == NULL)
        *new_head = pivot;
    
    *new_tail = end;
    
    return pivot;
}

QueueNode* quick_sort_rec(QueueNode *head, QueueNode *tail)
{
    if (!head || head == tail)
        return head;
    
    QueueNode *new_head = NULL, *new_tail = NULL;
    QueueNode *pivot = partition(head, tail, &new_head, &new_tail);
    
    if (new_head != pivot) {
        QueueNode *tmp = new_head;
        while (tmp->next != pivot)
            tmp = tmp->next;
        tmp->next = NULL;
        
        new_head = quick_sort_rec(new_head, tmp);
        
        tmp = new_head;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = pivot;
    }
    
    pivot->next = quick_sort_rec(pivot->next, new_tail);
    
    return new_head;
}

void queue_quick_sort(Queue *q)
{
    if (!q->head || !q->head->next)
        return;
    
    QueueNode *tail = q->head;
    while (tail->next)
        tail = tail->next;
    
    q->head = quick_sort_rec(q->head, tail);
    
    QueueNode *node = q->head;
    while (node && node->next) {
        node = node->next;
    }
    q->tail = node;
}

Queue* queue_copy(const Queue *q)
{
    Queue *copy = (Queue*)malloc(sizeof(Queue));
    if (!copy)
        return NULL;
    
    queue_init(copy);
    
    QueueNode *node = q->head;
    while (node) {
        if (queue_push(copy, node->value) != 0) {
            queue_free(copy);
            free(copy);
            return NULL;
        }
        node = node->next;
    }
    
    return copy;
}