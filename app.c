#include "app.h"
#include "queue.h"
#include "number_io.h"
#include "sort.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void handle_file_mode(const char *filename);
static void handle_sort_once(void);
static void handle_benchmark(void);

int run_app(int argc, char *argv[])
{
    /* режим с аргументом --file */
    if (argc == 3 && strcmp(argv[1], "--file") == 0) {
        handle_file_mode(argv[2]);
        return 0;
    }

    /* режим без аргументов – программа предлагает повторить работу */
    printf("Программа запущена без аргументов командной строки.\n");

    int done = 0;
    while (!done) {
        printf("\nМеню:\n");
        printf("1 - Ввести числа и отсортировать (метод прямого выбора)\n");
        printf("2 - Сравнение скоростей (выбор / быстрая сортировка Хоара)\n");
        printf("0 - Выход\n> ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Ошибка ввода, попробуйте еще раз.\n");
            continue;
        }
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}

        switch (choice) {
        case 1:
            handle_sort_once();
            break;
        case 2:
            handle_benchmark();
            break;
        case 0:
            done = 1;
            break;
        default:
            printf("Неизвестный пункт меню.\n");
            break;
        }

        if (!done) {
            printf("\nПовторить работу? (y/n): ");
            int ans = getchar();
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            if (ans != 'y' && ans != 'Y')
                done = 1;
        }
    }

    return 0;
}

/* ------------ режим с файлом ---------------- */

static void handle_file_mode(const char *filename)
{
    int   *prev_orig   = NULL;
    int   *prev_sorted = NULL;
    size_t prev_orig_n   = 0;
    size_t prev_sorted_n = 0;

    if (load_previous_rows(filename,
                           &prev_orig, &prev_orig_n,
                           &prev_sorted, &prev_sorted_n) == 0 &&
        prev_orig && prev_orig_n > 0)
    {
        printf("Ранее сохраненные данные из файла \"%s\":\n", filename);
        printf("Предыдущий введенный ряд:\n");
        print_int_array(prev_orig, prev_orig_n);

        if (prev_sorted && prev_sorted_n > 0) {
            printf("Предыдущий отсортированный ряд:\n");
            print_int_array(prev_sorted, prev_sorted_n);
        }
    } else {
        printf("Файл \"%s\" не найден или пуст. Будет создан новый файл.\n",
               filename);
    }

    free(prev_orig);
    free(prev_sorted);

    Queue q;
    queue_init(&q);

    int   *numbers = NULL;
    size_t count   = read_ints_from_stdin(&numbers);
    if (count == 0 || !numbers) {
        printf("Не удалось прочитать числа.\n");
        queue_free(&q);
        free(numbers);
        return;
    }

    for (size_t i = 0; i < count; ++i) {
        if (queue_push(&q, numbers[i]) != 0) {
            printf("Ошибка: не хватает памяти для очереди.\n");
            free(numbers);
            queue_free(&q);
            return;
        }
    }

    size_t arr_size = 0;
    int *data = queue_to_array(&q, &arr_size);
    if (!data && arr_size > 0) {
        printf("Ошибка: не удалось создать массив из очереди.\n");
        free(numbers);
        queue_free(&q);
        return;
    }

    selection_sort(data, arr_size);

    printf("\nНовый исходный ряд:\n");
    print_int_array(numbers, count);
    printf("Новый отсортированный ряд (метод прямого выбора):\n");
    print_int_array(data, arr_size);

    if (save_rows(filename, numbers, count, data, arr_size) != 0) {
        printf("Не удалось сохранить данные в файл \"%s\".\n", filename);
    } else {
        printf("Данные сохранены в файл \"%s\".\n", filename);
    }

    free(numbers);
    free(data);
    queue_free(&q);
}

/* ------------ разовая сортировка без файла --------- */

static void handle_sort_once(void)
{
    Queue q;
    queue_init(&q);

    int   *numbers = NULL;
    size_t count   = read_ints_from_stdin(&numbers);
    if (count == 0 || !numbers) {
        printf("Не удалось прочитать числа.\n");
        queue_free(&q);
        free(numbers);
        return;
    }

    for (size_t i = 0; i < count; ++i) {
        if (queue_push(&q, numbers[i]) != 0) {
            printf("Ошибка: не хватает памяти для очереди.\n");
            free(numbers);
            queue_free(&q);
            return;
        }
    }

    printf("\nИсходный ряд:\n");
    print_int_array(numbers, count);

    size_t arr_size = 0;
    int *data = queue_to_array(&q, &arr_size);
    if (!data && arr_size > 0) {
        printf("Ошибка: не удалось создать массив из очереди.\n");
        free(numbers);
        queue_free(&q);
        return;
    }

    selection_sort(data, arr_size);

    printf("Отсортированный ряд (метод прямого выбора):\n");
    print_int_array(data, arr_size);

    free(numbers);
    free(data);
    queue_free(&q);
}

/* ------------ сравнение скоростей ------------------ */

static void handle_benchmark(void)
{
    size_t n;
    printf("Введите размер массива для теста (например, 10000): ");
    if (scanf("%zu", &n) != 1 || n == 0) {
        printf("Некорректный размер.\n");
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        return;
    }
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}

    int *data = (int *)malloc(n * sizeof(int));
    int *copy = (int *)malloc(n * sizeof(int));
    if (!data || !copy) {
        printf("Не хватает памяти.\n");
        free(data);
        free(copy);
        return;
    }

    srand((unsigned)time(NULL));
    for (size_t i = 0; i < n; ++i)
        data[i] = rand();

    memcpy(copy, data, n * sizeof(int));

    clock_t start = clock();
    selection_sort(data, n);
    clock_t end = clock();
    double t_selection = (double)(end - start) / CLOCKS_PER_SEC;

    memcpy(data, copy, n * sizeof(int));

    start = clock();
    quick_sort(data, n);
    end   = clock();
    double t_quick = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\nРезультаты для %zu элементов:\n", n);
    printf("Метод прямого выбора: %.6f сек.\n", t_selection);
    printf("Быстрая сортировка (Хоара): %.6f сек.\n", t_quick);
    printf("Эти значения можно занести в таблицу и построить график.\n");

    free(data);
    free(copy);
}
