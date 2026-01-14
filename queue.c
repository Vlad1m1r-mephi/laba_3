#include "queue.h"      // Заголовочный файл с объявлением структур и функций очереди
#include <stdio.h>      // Для printf (используется в queue_print)
#include <stdlib.h>     // Для malloc, free (динамическое выделение памяти)
#include <time.h>       // Для clock() (время сортировки, если используется)

/*
 * Инициализация очереди
 * Устанавливает начальное состояние: пустая очередь
 * Сложность: O(1)
 */
void queue_init(Queue *q)
{
    q->head = q->tail = NULL;  // Нет первого и последнего элемента
    q->size = 0;                // Размер равен 0
}

/*
 * Добавление элемента в конец очереди (enqueue)
 * Выделяет память под новый узел и добавляет его в хвост
 * Сложность: O(1)
 * Возвращает: 0 при успехе, -1 при ошибке выделения памяти
 */
int queue_push(Queue *q, int value)
{
    // 1. Выделение памяти для нового узла
    QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
    if (!node)  // Проверка успешности выделения памяти
        return -1;

    // 2. Инициализация полей нового узла
    node->value = value;  // Сохраняем значение
    node->next = NULL;    // Это будет последний элемент

    // 3. Вставка узла в очередь
    if (q->tail) {
        // Если очередь не пуста: привязываем к предыдущему последнему элементу
        q->tail->next = node;
    } else {
        // Если очередь пуста: новый узел становится и head и tail
        q->head = node;
    }
    
    // 4. Обновление указателя на хвост и увеличение размера
    q->tail = node;  // Новый узел теперь последний
    q->size++;       // Увеличиваем счетчик элементов
    return 0;        // Успешное завершение
}

/*
 * Извлечение элемента из начала очереди (dequeue)
 * Удаляет первый узел и возвращает его значение
 * Сложность: O(1)
 * Возвращает: 0 при успехе, -1 если очередь пуста
 * value - указатель для сохранения извлеченного значения (может быть NULL)
 */
int queue_pop(Queue *q, int *value)
{
    // Проверка на пустую очередь
    if (!q->head)
        return -1;

    // 1. Получаем ссылку на первый узел
    QueueNode *node = q->head;
    
    // 2. Сохраняем значение, если запрошено
    if (value)
        *value = node->value;

    // 3. Перенаправляем указатель head на следующий узел
    q->head = node->next;
    
    // 4. Если после удаления очередь стала пустой, обнуляем tail
    if (!q->head)
        q->tail = NULL;

    // 5. Освобождаем память удаляемого узла
    free(node);
    
    // 6. Уменьшаем счетчик элементов
    q->size--;
    return 0;  // Успешное извлечение
}

/*
 * Полное освобождение памяти, занятой очередью
 * Последовательно удаляет все узлы
 * Сложность: O(n), где n - количество элементов
 */
void queue_free(Queue *q)
{
    QueueNode *cur = q->head;
    while (cur) {
        // Сохраняем ссылку на следующий узел перед удалением текущего
        QueueNode *next = cur->next;
        free(cur);        // Освобождаем текущий узел
        cur = next;       // Переходим к следующему
    }
    
    // Сбрасываем состояние очереди на начальное
    q->head = q->tail = NULL;
    q->size = 0;
}

/*
 * Вывод содержимого очереди в стандартный поток вывода
 * Формат: числа разделены пробелами, в конце новая строка
 * Сложность: O(n)
 */
void queue_print(const Queue *q)
{
    // Проверка на пустую очередь
    if (!q->head) {
        printf("Очередь пуста\n");
        return;
    }

    // Итерация по всем узлам очереди
    QueueNode *node = q->head;
    while (node) {
        printf("%d", node->value);  // Вывод значения
        node = node->next;          // Переход к следующему узлу
        
        // Вывод пробела между числами (кроме последнего)
        if (node)
            printf(" ");
    }
    printf("\n");  // Завершающая новая строка
}

/*
 * Изменение значения элемента по индексу (0-based)
 * Не меняет структуру очереди, только значение в существующем узле
 * Сложность: O(n) в худшем случае (если индекс близок к концу)
 * Возвращает: 0 при успехе, -1 при неверном индексе
 */
int queue_edit_at(Queue *q, size_t index, int new_value)
{
    // Проверка корректности индекса
    if (index >= q->size)
        return -1;

    // Поиск узла по индексу
    QueueNode *node = q->head;
    for (size_t i = 0; i < index; i++) {
        node = node->next;  // Линейный поиск
    }
    
    // Изменение значения найденного узла
    node->value = new_value;
    return 0;  // Успешное изменение
}

/*
 * Проверка очереди на пустоту
 * Сложность: O(1)
 * Возвращает: 1 если очередь пуста, 0 если содержит элементы
 */
int queue_is_empty(const Queue *q)
{
    return q->head == NULL;  // Очередь пуста, если head равен NULL
}

/*
 * Сортировка очереди методом прямого выбора (selection sort)
 * Алгоритм: на каждом шаге находим минимальный элемент в неотсортированной части
 *           и меняем его с первым элементом неотсортированной части
 * Сложность: O(n²) - квадратичная сложность
 * Особенности: сортировка на месте (in-place), устойчивая
 */
void queue_selection_sort(Queue *q)
{
    if (!q->head || !q->head->next)
        return;

    QueueNode *prev_current = NULL;
    QueueNode *current = q->head;
    QueueNode *new_head = NULL;
    QueueNode *new_tail = NULL;
    
    while (current) {
        // Находим минимальный элемент в оставшейся части
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
        
        // Удаляем min из текущей позиции
        if (prev_min) {
            prev_min->next = min->next;
        } else {
            current = min->next;  // min был current
        }
        
        // Отсоединяем min
        min->next = NULL;
        
        // Добавляем min в новый отсортированный список
        if (!new_head) {
            new_head = new_tail = min;
        } else {
            new_tail->next = min;
            new_tail = min;
        }
        
        // Если min был current, нужно обновить current
        if (min == current) {
            if (prev_current) {
                prev_current->next = current->next;
                current = current->next;
            } else {
                current = q->head->next;
            }
        }
    }
    
    // Обновляем указатели очереди
    q->head = new_head;
    q->tail = new_tail;
}

/*
 * Вспомогательная функция: разделение очереди относительно опорного элемента
 * Возвращает узел, который будет новым хвостом левой части
 */
QueueNode* partition(QueueNode *head, QueueNode *tail, 
                     QueueNode **new_head, QueueNode **new_tail)
{
    QueueNode *pivot = tail;
    QueueNode *prev = NULL, *cur = head, *end = pivot;
    
    while (cur != pivot) {
        if (cur->value < pivot->value) {
            // Оставляем в левой части
            if (*new_head == NULL)
                *new_head = cur;
            prev = cur;
            cur = cur->next;
        } else {
            // Перемещаем в правую часть
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

/*
 * Рекурсивная сортировка
 */
QueueNode* quick_sort_recursive(QueueNode *head, QueueNode *tail)
{
    if (!head || head == tail)
        return head;
    
    QueueNode *new_head = NULL, *new_tail = NULL;
    
    // Разделяем список
    QueueNode *pivot = partition(head, tail, &new_head, &new_tail);
    
    // Если элементы слева от pivot есть
    if (new_head != pivot) {
        // Отделяем левую часть
        QueueNode *tmp = new_head;
        while (tmp->next != pivot)
            tmp = tmp->next;
        tmp->next = NULL;
        
        // Рекурсивно сортируем левую часть
        new_head = quick_sort_recursive(new_head, tmp);
        
        // Соединяем с pivot
        tmp = get_tail(new_head);
        tmp->next = pivot;
    }
    
    // Рекурсивно сортируем правую часть
    pivot->next = quick_sort_recursive(pivot->next, new_tail);
    
    return new_head;
}

QueueNode* get_tail(QueueNode *head)
{
    while (head && head->next)
        head = head->next;
    return head;
}

/*
 * Быстрая сортировка (Хоара) для очереди
 * Алгоритм: рекурсивное разделение относительно опорного элемента
 * Средняя сложность: O(n log n)
 * Худшая сложность: O(n²) при неудачном выборе опорного элемента
 * Особенности: не требует дополнительной памяти для массива индексов
 */
void queue_quick_sort(Queue *q)
{
    if (!q->head || !q->head->next)
        return;
    
    // Сортируем рекурсивно
    q->head = quick_sort_recursive(q->head, q->tail);
    
    // Обновляем tail
    q->tail = get_tail(q->head);
}

/*
 * Создание глубокой копии очереди
 * Выделяет память под новую очередь и все её узлы
 * Сложность: O(n)
 * Возвращает: указатель на новую очередь или NULL при ошибке
 */
Queue* queue_copy(const Queue *q)
{
    // 1. Выделение памяти для структуры очереди
    Queue *copy = (Queue*)malloc(sizeof(Queue));
    if (!copy)
        return NULL;
    
    // 2. Инициализация новой очереди
    queue_init(copy);
    
    // 3. Последовательное копирование всех узлов
    QueueNode *node = q->head;
    while (node) {
        // Добавление копии значения в новую очередь
        if (queue_push(copy, node->value) != 0) {
            // При ошибке: освобождаем уже скопированное
            queue_free(copy);
            free(copy);
            return NULL;
        }
        node = node->next;
    }
    
    // 4. Возврат указателя на копию
    return copy;
}