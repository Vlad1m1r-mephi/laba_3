#ifndef QUEUE_H  
#define QUEUE_H 

#include <stddef.h>  // Для типа size_t (беззнаковый целый тип для хранения размеров)

//ОЧЕРЕДЬ (QUEUE) - структура данных типа FIFO (First In, First Out)


//УЗЕЛ ОЧЕРЕДИ (QueueNode)
typedef struct QueueNode {
    int value;                    // Хранимое целочисленное значение
    struct QueueNode *next;       // Указатель на следующий узел в очереди
} QueueNode;


//СТРУКТУРА ОЧЕРЕДИ (Queue)
typedef struct Queue {
    QueueNode *head;  // Указатель на первый элемент (для извлечения)
    QueueNode *tail;  // Указатель на последний элемент (для добавления)
    unsigned int size;      // Количество элементов в очереди
} Queue;

/* ==================== БАЗОВЫЕ ОПЕРАЦИИ ==================== */

//Инициализация очереди
void queue_init(Queue *q);

//Добавление элемента в конец очереди (enqueue)
int queue_push(Queue *q, int value);

//Извлечение элемента из начала очереди (dequeue)
int queue_pop(Queue *q, int *value);

//Освобождение памяти, занятой очередью
void queue_free(Queue *q);

//Вывод содержимого очереди в стандартный поток вывода
void queue_print(const Queue *q);

/* ==================== ДОПОЛНИТЕЛЬНЫЕ ОПЕРАЦИИ ==================== */

//Изменение значения элемента по индексу
int queue_edit_at(Queue *q, size_t index, int new_value);

//Сортировка очереди методом прямого выбора (selection sort)
void queue_selection_sort(Queue *q);

//Сортировка очереди методом быстрой сортировки (quick sort)
void queue_quick_sort(Queue *q);

//Проверка очереди на пустоту
int queue_is_empty(const Queue *q);

QueueNode* get_tail(QueueNode *head);

Queue* queue_copy(const Queue *q);

#endif