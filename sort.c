#include "sort.h"

void selection_sort(int *data, size_t n)
{
    for (size_t i = 0; i + 1 < n; ++i) {
        size_t min_idx = i;
        for (size_t j = i + 1; j < n; ++j) {
            if (data[j] < data[min_idx])
                min_idx = j;
        }
        if (min_idx != i) {
            int tmp      = data[i];
            data[i]      = data[min_idx];
            data[min_idx] = tmp;
        }
    }
}

/* классический Hoare quicksort (через рекурсивную функцию) */
static void quick_sort_recursive(int *data, long left, long right)
{
    if (left >= right)
        return;

    int  pivot = data[(left + right) / 2];
    long i = left - 1;
    long j = right + 1;

    for (;;) {
        do { ++i; } while (data[i] < pivot);
        do { --j; } while (data[j] > pivot);
        if (i >= j)
            break;

        int tmp = data[i];
        data[i] = data[j];
        data[j] = tmp;
    }

    quick_sort_recursive(data, left,  j);
    quick_sort_recursive(data, j + 1, right);
}

void quick_sort(int *data, size_t n)
{
    if (n == 0)
        return;
    quick_sort_recursive(data, 0, (long)n - 1);
}
