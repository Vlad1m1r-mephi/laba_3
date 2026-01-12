#include "number_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER_SIZE 4096
#define INITIAL_CAPACITY 16

/* чтение одной строки чисел из любого потока */
static size_t read_ints_from_stream(FILE *stream, int **out_data)
{
    char buffer[LINE_BUFFER_SIZE];

    if (!fgets(buffer, sizeof(buffer), stream)) {
        *out_data = NULL;
        return 0;
    }

    int   *data     = NULL;
    size_t size     = 0;
    size_t capacity = 0;

    char *token = strtok(buffer, " \t\r\n");
    while (token) {
        if (size == capacity) {
            size_t new_cap = capacity ? capacity * 2 : INITIAL_CAPACITY;
            int *tmp = (int *)realloc(data, new_cap * sizeof(int));
            if (!tmp) {
                free(data);
                *out_data = NULL;
                return 0;
            }
            data     = tmp;
            capacity = new_cap;
        }

        data[size++] = atoi(token);
        token = strtok(NULL, " \t\r\n");
    }

    *out_data = data;
    return size;
}

size_t read_ints_from_stdin(int **out_data)
{
    printf("Введите последовательность целых чисел через пробел:\n> ");
    return read_ints_from_stream(stdin, out_data);
}

void print_int_array(const int *data, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        printf("%d", data[i]);
        if (i + 1 < n)
            printf(" ");
    }
    printf("\n");
}

int load_previous_rows(const char *filename,
                       int **prev_original, size_t *prev_original_n,
                       int **prev_sorted,   size_t *prev_sorted_n)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return -1;  /* файла нет – это не ошибка программы */

    int   *orig   = NULL;
    int   *sorted = NULL;
    size_t orig_n   = read_ints_from_stream(f, &orig);
    size_t sorted_n = read_ints_from_stream(f, &sorted);

    fclose(f);

    if (prev_original) {
        *prev_original = orig;
        if (prev_original_n)
            *prev_original_n = orig_n;
    } else {
        free(orig);
    }

    if (prev_sorted) {
        *prev_sorted = sorted;
        if (prev_sorted_n)
            *prev_sorted_n = sorted_n;
    } else {
        free(sorted);
    }

    return 0;
}

int save_rows(const char *filename,
              const int *original, size_t original_n,
              const int *sorted,   size_t sorted_n)
{
    FILE *f = fopen(filename, "w");
    if (!f)
        return -1;

    for (size_t i = 0; i < original_n; ++i) {
        fprintf(f, "%d", original[i]);
        if (i + 1 < original_n)
            fprintf(f, " ");
    }
    fprintf(f, "\n");

    for (size_t i = 0; i < sorted_n; ++i) {
        fprintf(f, "%d", sorted[i]);
        if (i + 1 < sorted_n)
            fprintf(f, " ");
    }
    fprintf(f, "\n");

    fclose(f);
    return 0;
}
