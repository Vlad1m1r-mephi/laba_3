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
    // Проверка тривиальных случаев: 0 или 1 элемент
    if (!q->head || !q->head->next)
        return;

    // Внешний цикл: проходим по всем элементам
    QueueNode *current = q->head;
    
    while (current) {
        // Предполагаем, что текущий элемент - минимальный
        QueueNode *min = current;
        QueueNode *runner = current->next;
        
        // Внутренний цикл: поиск минимального элемента в оставшейся части
        while (runner) {
            if (runner->value < min->value) {
                min = runner;  // Найден новый минимум
            }
            runner = runner->next;
        }
        
        // Если минимальный элемент не на своем месте, меняем значения
        if (min != current) {
            int temp = current->value;
            current->value = min->value;
            min->value = temp;
        }
        
        // Переходим к следующей позиции
        current = current->next;
    }
}

/*
 * Вспомогательная функция для быстрой сортировки
 * Разделяет список относительно опорного элемента (pivot)
 * Элементы меньше pivot перемещаются в начало, остальные - в конец
 * Возвращает указатель на опорный элемент
 * new_head, new_tail - обновленные границы после разделения
 */
QueueNode* partition(QueueNode *head, QueueNode *tail, QueueNode **new_head, QueueNode **new_tail)
{
    QueueNode *pivot = tail;           // Опорный элемент - последний в текущем сегменте
    QueueNode *prev = NULL, *cur = head, *end = pivot;
    
    while (cur != pivot) {
        if (cur->value < pivot->value) {
            // Элемент меньше опорного - остается на своем месте
            if (*new_head == NULL)
                *new_head = cur;
            prev = cur;
            cur = cur->next;
        } else {
            // Элемент больше или равен опорному - перемещается в конец
            if (prev)
                prev->next = cur->next;
            QueueNode *tmp = cur->next;
            cur->next = NULL;
            end->next = cur;
            end = cur;
            cur = tmp;
        }
    }
    
    // Если все элементы были меньше опорного
    if (*new_head == NULL)
        *new_head = pivot;
    
    // Обновление хвоста
    *new_tail = end;
    
    return pivot;
}

/*
 * Рекурсивная функция быстрой сортировки для связного списка
 * Применяет стратегию "разделяй и властвуй"
 * Возвращает новый указатель на начало отсортированного списка
 */
QueueNode* quick_sort_rec(QueueNode *head, QueueNode *tail)
{
    // Базовый случай рекурсии: 0 или 1 элемент
    if (!head || head == tail)
        return head;
    
    QueueNode *new_head = NULL, *new_tail = NULL;
    
    // 1. Разделение: pivot занимает правильную позицию
    QueueNode *pivot = partition(head, tail, &new_head, &new_tail);
    
    // 2. Рекурсивная сортировка левой части (элементы < pivot)
    if (new_head != pivot) {
        // Отделяем левую часть от pivot
        QueueNode *tmp = new_head;
        while (tmp->next != pivot)
            tmp = tmp->next;
        tmp->next = NULL;
        
        // Рекурсивная сортировка левой части
        new_head = quick_sort_rec(new_head, tmp);
        
        // Присоединяем отсортированную левую часть к pivot
        tmp = new_head;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = pivot;
    }
    
    // 3. Рекурсивная сортировка правой части (элементы > pivot)
    pivot->next = quick_sort_rec(pivot->next, new_tail);
    
    // 4. Возврат нового начала списка
    return new_head;
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
    // Проверка тривиальных случаев
    if (!q->head || !q->head->next)
        return;
    
    // Находим хвост очереди (последний элемент)
    QueueNode *tail = q->head;
    while (tail->next)
        tail = tail->next;
    
    // Рекурсивная сортировка
    q->head = quick_sort_rec(q->head, tail);
    
    // Обновление указателя на хвост после сортировки
    QueueNode *node = q->head;
    while (node && node->next) {
        node = node->next;
    }
    q->tail = node;
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