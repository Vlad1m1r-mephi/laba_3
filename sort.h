#ifndef SORT_H
#define SORT_H

#include <stddef.h>

void selection_sort(int *data, size_t n); /* метод прямого выбора */
void quick_sort(int *data, size_t n);     /* быстрая сортировка Хоара */

#endif /* SORT_H */
