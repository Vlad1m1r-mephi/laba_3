#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Инициализация пустой очереди
void queue_init(Queue *q)
{
    q->head = q->tail = NULL;
    q->size = 0;
}

// Добавление элемента в конец очереди
// Возвращает 0 при успехе, -1 при ошибке
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

// Извлечение элемента из начала очереди
// Если value не NULL, сохраняет извлеченное значение
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

// Освобождение всей памяти, занятой очередью
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

// Печать содержимого очереди
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

// Изменение значения элемента по индексу
// Индексация с нуля
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

// Проверка, пуста ли очередь
int queue_is_empty(const Queue *q)
{
    return q->head == NULL;
}

// Сортировка очереди методом прямого выбора
// Работает за O(n²), но прост в реализации
void queue_selection_sort(Queue *q)
{
    if (!q->head || !q->head->next)
        return;

    QueueNode *prev_current = NULL;
    QueueNode *current = q->head;
    QueueNode *new_head = NULL;
    QueueNode *new_tail = NULL;
    
    while (current) {
        // Ищем минимальный элемент в оставшейся части
        QueueNode *prev_min = NULL;
        QueueNode *min = current;
        QueueNode *prev = current;
        QueueNode *runner = current->next;
        
        while (runner) {
            if (runner->value < min->value) {
                prev_min = prev;
                min = runner;
            }
            prev = runner;
            runner = runner->next;
        }
        
        // Убираем минимальный элемент из текущего списка
        if (prev_min) {
            prev_min->next = min->next;
        } else {
            current = min->next;
        }
        
        min->next = NULL;
        
        // Добавляем его в новый отсортированный список
        if (!new_head) {
            new_head = new_tail = min;
        } else {
            new_tail->next = min;
            new_tail = min;
        }
        
        // Нужно аккуратно обработать случай, когда min был current
        if (min == current) {
            if (prev_current) {
                prev_current->next = current->next;
                current = current->next;
            } else {
                current = q->head->next;
            }
        }
    }
    
    q->head = new_head;
    q->tail = new_tail;
}

// Вспомогательная функция для быстрой сортировки
// Разделяет список относительно опорного элемента
QueueNode* partition(QueueNode *head, QueueNode *tail, 
                     QueueNode **new_head, QueueNode **new_tail)
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
            // Перемещаем в правую часть (после опорного)
            QueueNode *tmp = cur->next;
            
            if (prev)
                prev->next = cur->next;
            
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

// Рекурсивная часть быстрой сортировки
QueueNode* quick_sort_recursive(QueueNode *head, QueueNode *tail)
{
    if (!head || head == tail)
        return head;
    
    QueueNode *new_head = NULL, *new_tail = NULL;
    
    QueueNode *pivot = partition(head, tail, &new_head, &new_tail);
    
    // Сортируем левую часть (если она есть)
    if (new_head != pivot) {
        QueueNode *tmp = new_head;
        while (tmp->next != pivot)
            tmp = tmp->next;
        tmp->next = NULL;
        
        new_head = quick_sort_recursive(new_head, tmp);
        
        tmp = get_tail(new_head);
        tmp->next = pivot;
    }
    
    // Сортируем правую часть
    pivot->next = quick_sort_recursive(pivot->next, new_tail);
    
    return new_head;
}

// Получить последний элемент списка
QueueNode* get_tail(QueueNode *head)
{
    while (head && head->next)
        head = head->next;
    return head;
}

// Быстрая сортировка для очереди
// В среднем работает за O(n log n), что быстрее сортировки выбором
void queue_quick_sort(Queue *q)
{
    if (!q->head || !q->head->next)
        return;
    
    q->head = quick_sort_recursive(q->head, q->tail);
    q->tail = get_tail(q->head);
}
