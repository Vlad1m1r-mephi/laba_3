#include "app.h"
#include "queue.h"
#include "number_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

void handle_file_mode(const char *filename);
void handle_sort_once(void);
void handle_benchmark(void);
void handle_edit_element(void);
void handle_queue_operations(void);
void print_queue_stats(const Queue *q);
void benchmark_automated(void);
int safe_scanf_int(int *value);
int safe_scanf_size_t(size_t *value);

int run_app(int argc, char *argv[])
{
    // Настройка кодировки консоли
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    #else
    setlocale(LC_ALL, "Russian");
    #endif
    
    // Обработка аргументов командной строки
    if (argc == 3 && strcmp(argv[1], "--file") == 0) {
        handle_file_mode(argv[2]);
        return 0;
    }
    
    if (argc == 2 && strcmp(argv[1], "--benchmark-auto") == 0) {
        benchmark_automated();
        return 0;
    }

    printf("Программа для работы с очередью и сортировкой\n");
    printf("============================================\n");

    int done = 0;
    while (!done) {
        printf("\nМеню:\n");
        printf("1 - Создать очередь и отсортировать (метод прямого выбора)\n");
        printf("2 - Сравнение скоростей сортировок (очередь)\n");
        printf("3 - Редактировать элемент очереди\n");
        printf("4 - Основные операции с очередью\n");
        printf("5 - Работа с файлом\n");
        printf("0 - Выход\n> ");

        int choice;
        if (!safe_scanf_int(&choice)) {
            printf("Ошибка ввода, попробуйте еще раз.\n");
            continue;
        }
        
        // Очистка буфера ввода
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}

        switch (choice) {
        case 1:
            handle_sort_once();
            break;
        case 2:
            handle_benchmark();
            break;
        case 3:
            handle_edit_element();
            break;
        case 4:
            handle_queue_operations();
            break;
        case 5:
            {
                char filename[256];
                printf("Введите имя файла: ");
                if (fgets(filename, sizeof(filename), stdin) == NULL) {
                    printf("Ошибка ввода имени файла.\n");
                    break;
                }
                filename[strcspn(filename, "\n")] = 0;
                if (strlen(filename) == 0) {
                    printf("Имя файла не может быть пустым.\n");
                    break;
                }
                handle_file_mode(filename);
            }
            break;
        case 0:
            done = 1;
            printf("Выход из программы.\n");
            break;
        default:
            printf("Неизвестный пункт меню.\n");
            break;
        }

        // Подтверждение продолжения работы
        if (!done) {
            printf("\nВернуться в меню? (y/n): ");
            int ans = getchar();
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            if (ans != 'y' && ans != 'Y')
                done = 1;
        }
    }

    return 0;
}

// Безопасный ввод целого числа
int safe_scanf_int(int *value)
{
    int result = scanf("%d", value);
    if (result != 1) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        return 0;
    }
    return 1;
}

// Безопасный ввод size_t
int safe_scanf_size_t(size_t *value)
{
    int result = scanf("%zu", value);
    if (result != 1) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        return 0;
    }
    return 1;
}

// Режим работы с файлом
void handle_file_mode(const char *filename)
{
    int *prev_orig = NULL, *prev_sorted = NULL;
    size_t prev_orig_n = 0, prev_sorted_n = 0;

    // Загрузка ранее сохраненных данных
    if (load_previous_rows(filename, &prev_orig, &prev_orig_n,
                          &prev_sorted, &prev_sorted_n) == 0 &&
        prev_orig && prev_orig_n > 0) {
        printf("\nРанее сохраненные данные из файла \"%s\":\n", filename);
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

    // Создание и заполнение очереди
    Queue q;
    queue_init(&q);

    printf("\nВведите новую последовательность чисел через пробел:\n> ");
    int *numbers = NULL;
    size_t count = read_ints_from_stdin(&numbers);
    
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

    printf("\nСозданная очередь:\n");
    queue_print(&q);

    // Создание копии для сортировки
    Queue *q_copy = queue_copy(&q);
    if (!q_copy) {
        printf("Ошибка копирования очереди.\n");
        free(numbers);
        queue_free(&q);
        return;
    }

    printf("\nСортируем очередь методом прямого выбора...\n");
    queue_selection_sort(q_copy);

    // Вывод результатов
    printf("\nИсходная очередь:\n");
    queue_print(&q);
    printf("Отсортированная очередь:\n");
    queue_print(q_copy);

    // Подготовка массивов для сохранения в файл
    int *orig_array = (int*)malloc(q.size * sizeof(int));
    int *sorted_array = (int*)malloc(q_copy->size * sizeof(int));
    
    if (orig_array && sorted_array) {
        // Копирование данных из очереди в массивы
        QueueNode *node = q.head;
        size_t i = 0;
        while (node) {
            orig_array[i++] = node->value;
            node = node->next;
        }
        
        node = q_copy->head;
        i = 0;
        while (node) {
            sorted_array[i++] = node->value;
            node = node->next;
        }
        
        // Сохранение в файл
        if (save_rows(filename, orig_array, q.size, sorted_array, q_copy->size) != 0) {
            printf("Не удалось сохранить данные в файл \"%s\".\n", filename);
        } else {
            printf("Данные сохранены в файл \"%s\".\n", filename);
        }
    } else {
        printf("Ошибка выделения памяти для сохранения.\n");
    }
    
    // Очистка памяти
    free(orig_array);
    free(sorted_array);
    free(numbers);
    queue_free(&q);
    queue_free(q_copy);
    free(q_copy);
}

// Однократная сортировка очереди
void handle_sort_once(void)
{
    Queue q;
    queue_init(&q);

    printf("Введите последовательность целых чисел через пробел:\n> ");
    int *numbers = NULL;
    size_t count = read_ints_from_stdin(&numbers);
    
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

    printf("\nИсходная очередь:\n");
    queue_print(&q);

    queue_selection_sort(&q);

    printf("Отсортированная очередь (метод прямого выбора):\n");
    queue_print(&q);

    free(numbers);
    queue_free(&q);
}

// Редактирование элемента очереди
void handle_edit_element(void)
{
    Queue q;
    queue_init(&q);

    printf("Введите последовательность целых чисел через пробел:\n> ");
    int *numbers = NULL;
    size_t count = read_ints_from_stdin(&numbers);
    
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

    printf("\nТекущая очередь (%zu элементов):\n", q.size);
    queue_print(&q);

    if (q.size > 0) {
        size_t index;
        int new_value;
        
        printf("Введите индекс элемента для редактирования (0-%zu): ", q.size - 1);
        if (!safe_scanf_size_t(&index)) {
            printf("Ошибка ввода индекса.\n");
            free(numbers);
            queue_free(&q);
            return;
        }
        
        printf("Введите новое значение: ");
        if (!safe_scanf_int(&new_value)) {
            printf("Ошибка ввода значения.\n");
            free(numbers);
            queue_free(&q);
            return;
        }

        if (queue_edit_at(&q, index, new_value) == 0) {
            printf("Элемент успешно изменен. Новая очередь:\n");
            queue_print(&q);
        } else {
            printf("Ошибка: неверный индекс.\n");
        }
    }

    free(numbers);
    queue_free(&q);
}

// Меню операций с очередью
void handle_queue_operations(void)
{
    Queue q;
    queue_init(&q);
    
    int done = 0;
    while (!done) {
        printf("\nОперации с очередью:\n");
        printf("1 - Добавить элемент\n");
        printf("2 - Удалить элемент (из начала)\n");
        printf("3 - Вывести очередь\n");
        printf("4 - Редактировать элемент\n");
        printf("5 - Очистить очередь\n");
        printf("6 - Вывести статистику\n");
        printf("0 - Назад\n> ");
        
        int choice;
        if (!safe_scanf_int(&choice)) {
            printf("Ошибка ввода, попробуйте еще раз.\n");
            continue;
        }
        getchar();
        
        switch (choice) {
        case 1: {
            int value;
            printf("Введите значение: ");
            if (!safe_scanf_int(&value)) {
                printf("Ошибка ввода значения.\n");
                break;
            }
            getchar();
            if (queue_push(&q, value) == 0) {
                printf("Элемент %d добавлен.\n", value);
            } else {
                printf("Ошибка добавления.\n");
            }
            break;
        }
        case 2: {
            int value;
            if (queue_pop(&q, &value) == 0) {
                printf("Удален элемент: %d\n", value);
            } else {
                printf("Очередь пуста.\n");
            }
            break;
        }
        case 3:
            printf("Содержимое очереди:\n");
            queue_print(&q);
            break;
        case 4: {
            if (q.size == 0) {
                printf("Очередь пуста.\n");
                break;
            }
            size_t index;
            int new_value;
            printf("Введите индекс (0-%zu): ", q.size - 1);
            if (!safe_scanf_size_t(&index)) {
                printf("Ошибка ввода индекса.\n");
                break;
            }
            printf("Введите новое значение: ");
            if (!safe_scanf_int(&new_value)) {
                printf("Ошибка ввода значения.\n");
                break;
            }
            getchar();
            if (queue_edit_at(&q, index, new_value) == 0) {
                printf("Элемент изменен.\n");
            } else {
                printf("Неверный индекс.\n");
            }
            break;
        }
        case 5:
            queue_free(&q);
            queue_init(&q);
            printf("Очередь очищена.\n");
            break;
        case 6:
            print_queue_stats(&q);
            break;
        case 0:
            done = 1;
            break;
        default:
            printf("Неизвестная операция.\n");
            break;
        }
    }
    
    queue_free(&q);
}

// Вывод статистики очереди
void print_queue_stats(const Queue *q)
{
    printf("\nСтатистика очереди:\n");
    printf("Количество элементов: %zu\n", q->size);
    printf("Состояние: %s\n", queue_is_empty(q) ? "пуста" : "не пуста");
    
    if (!queue_is_empty(q)) {
        printf("Первый элемент: %d\n", q->head->value);
        printf("Последний элемент: %d\n", q->tail->value);
        printf("Содержимое: ");
        queue_print(q);
    }
}

// Интерактивное тестирование скорости сортировок
void handle_benchmark(void)
{
    size_t n;
    printf("Введите размер тестовой очереди (например, 10000): ");
    if (!safe_scanf_size_t(&n) || n == 0) {
        printf("Некорректный размер.\n");
        return;
    }
    getchar();

    Queue q1, q2;
    queue_init(&q1);
    queue_init(&q2);

    printf("Генерируем %zu случайных чисел...\n", n);
    srand((unsigned)time(NULL));
    for (size_t i = 0; i < n; ++i) {
        int val = rand() % 1000000;
        queue_push(&q1, val);
        queue_push(&q2, val);
    }

    // Тестирование сортировки выбором
    printf("Тестирование сортировки выбором...\n");
    clock_t start = clock();
    queue_selection_sort(&q1);
    clock_t end = clock();
    double t_selection = (double)(end - start) / CLOCKS_PER_SEC;

    // Тестирование быстрой сортировки
    printf("Тестирование быстрой сортировки...\n");
    start = clock();
    queue_quick_sort(&q2);
    end = clock();
    double t_quick = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\nРезультаты для очереди из %zu элементов:\n", n);
    printf("Метод прямого выбора: %.6f сек.\n", t_selection);
    printf("Быстрая сортировка (Хоара): %.6f сек.\n", t_quick);
    printf("Отношение скоростей: %.2f:1 (быстрая быстрее в %.2f раз)\n", 
           t_selection / t_quick, t_selection / t_quick);

    // Сохранение результатов в файл
    FILE *f = fopen("benchmark_results.txt", "a");
    if (f) {
        fprintf(f, "%zu %.6f %.6f %.2f\n", n, t_selection, t_quick, t_selection/t_quick);
        fclose(f);
        printf("\nРезультаты добавлены в файл 'benchmark_results.txt'\n");
    }

    queue_free(&q1);
    queue_free(&q2);
}

// Автоматическое тестирование на нескольких размерах
void benchmark_automated(void)
{
    printf("Автоматическое тестирование алгоритмов сортировки\n");
    printf("================================================\n\n");
    
    // Создание папки для результатов
    #ifdef _WIN32
    if (_mkdir("benchmark_results") != 0) {
        if (errno != EEXIST) {
            printf("Ошибка создания папки benchmark_results\n");
            return;
        }
    }
    #else
    if (mkdir("benchmark_results", 0755) != 0) {
        if (errno != EEXIST) {
            printf("Ошибка создания папки benchmark_results\n");
            return;
        }
    }
    #endif
    
    // Формирование имени файла с временной меткой
    char json_filename[256];
    char timestamp[64];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", t);
    snprintf(json_filename, sizeof(json_filename), 
             "benchmark_results/benchmark_%s.json", timestamp);
    
    FILE *f_json = fopen(json_filename, "w");
    if (!f_json) {
        printf("Ошибка создания файла %s\n", json_filename);
        return;
    }
    
    // Запись заголовка JSON
    fprintf(f_json, "{\n");
    fprintf(f_json, "  \"title\": \"Сравнение алгоритмов сортировки очереди\",\n");
    fprintf(f_json, "  \"description\": \"Результаты тестирования на разных размерах данных\",\n");
    fprintf(f_json, "  \"timestamp\": \"%s\",\n", timestamp);
    fprintf(f_json, "  \"algorithm_names\": {\n");
    fprintf(f_json, "    \"selection\": \"Сортировка выбором (O(n²))\",\n");
    fprintf(f_json, "    \"quick\": \"Быстрая сортировка (O(n log n))\"\n");
    fprintf(f_json, "  },\n");
    fprintf(f_json, "  \"sizes\": [");
    
    size_t sizes[] = {100, 500, 1000, 5000, 10000, 20000, 50000, 75000, 100000, 150000, 200000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    double *selection_times = (double*)malloc(num_sizes * sizeof(double));
    double *quick_times = (double*)malloc(num_sizes * sizeof(double));
    double *ratios = (double*)malloc(num_sizes * sizeof(double));
    
    if (!selection_times || !quick_times || !ratios) {
        printf("Ошибка выделения памяти для результатов\n");
        fclose(f_json);
        free(selection_times);
        free(quick_times);
        free(ratios);
        return;
    }
    
    printf("Запуск тестов для %d различных размеров...\n\n", num_sizes);
    
    // Запись размеров в JSON
    for (int i = 0; i < num_sizes; i++) {
        fprintf(f_json, "%zu", sizes[i]);
        if (i < num_sizes - 1) fprintf(f_json, ", ");
    }
    fprintf(f_json, "],\n");
    
    fprintf(f_json, "  \"selection_sort\": [");
    fclose(f_json);
    
    // Цикл тестирования для каждого размера
    for (int i = 0; i < num_sizes; i++) {
        size_t n = sizes[i];
        printf("Тест %d/%d: размер = %zu\n", i+1, num_sizes, n);
        
        Queue q1, q2;
        queue_init(&q1);
        queue_init(&q2);
        
        srand((unsigned)time(NULL) + i);
        printf("   Генерация %zu случайных чисел... ", n);
        fflush(stdout);
        
        for (size_t j = 0; j < n; ++j) {
            int val = rand() % 1000000;
            queue_push(&q1, val);
            queue_push(&q2, val);
        }
        printf("\n");
        
        // Сортировка выбором
        printf("   Сортировка выбором... ");
        fflush(stdout);
        clock_t start = clock();
        queue_selection_sort(&q1);
        clock_t end = clock();
        double t_selection = (double)(end - start) / CLOCKS_PER_SEC;
        
        if (t_selection < 0.000001) t_selection = 0.000001;
        printf("%.6f сек\n", t_selection);
        
        // Быстрая сортировка
        printf("   Быстрая сортировка... ");
        fflush(stdout);
        start = clock();
        queue_quick_sort(&q2);
        end = clock();
        double t_quick = (double)(end - start) / CLOCKS_PER_SEC;
        
        if (t_quick < 0.000001) t_quick = 0.000001;
        printf("%.6f сек\n", t_quick);
        
        selection_times[i] = t_selection;
        quick_times[i] = t_quick;
        
        if (t_quick > 0.000001) {
            ratios[i] = t_selection / t_quick;
        } else {
            ratios[i] = 0.0;
        }
        
        printf("   Отношение: %.2f:1\n\n", ratios[i]);
        
        queue_free(&q1);
        queue_free(&q2);
    }
    
    // Запись результатов сортировки выбором
    f_json = fopen(json_filename, "a");
    if (f_json) {
        for (int i = 0; i < num_sizes; i++) {
            fprintf(f_json, "%.6f", selection_times[i]);
            if (i < num_sizes - 1) fprintf(f_json, ", ");
        }
        fprintf(f_json, "],\n");
        fprintf(f_json, "  \"quick_sort\": [");
        fclose(f_json);
    }
    
    // Запись результатов быстрой сортировки
    f_json = fopen(json_filename, "a");
    if (f_json) {
        for (int i = 0; i < num_sizes; i++) {
            fprintf(f_json, "%.6f", quick_times[i]);
            if (i < num_sizes - 1) fprintf(f_json, ", ");
        }
        fprintf(f_json, "],\n");
        fprintf(f_json, "  \"ratios\": [");
        fclose(f_json);
    }
    
    // Запись отношений скоростей
    f_json = fopen(json_filename, "a");
    if (f_json) {
        for (int i = 0; i < num_sizes; i++) {
            if (ratios[i] != ratios[i] || ratios[i] < 0 || ratios[i] > 1e100) {
                fprintf(f_json, "0.0");
            } else {
                fprintf(f_json, "%.2f", ratios[i]);
            }
            
            if (i < num_sizes - 1) fprintf(f_json, ", ");
        }
        fprintf(f_json, "]\n");
        fprintf(f_json, "}\n");
        fclose(f_json);
    }
    
    free(selection_times);
    free(quick_times);
    free(ratios);
    
    printf("Тестирование завершено!\n");
    printf("Данные сохранены в: '%s'\n\n", json_filename);
    
    // Сохранение имени последнего файла
    FILE *f_last = fopen("benchmark_results/last_benchmark.txt", "w");
    if (f_last) {
        fprintf(f_last, "%s", json_filename);
        fclose(f_last);
    }
    
    printf("Для визуализации результатов выполните:\n");
    printf("   python plot_benchmark_results.py\n\n");
    
    // Проверка наличия Python скрипта
    FILE *test_py = fopen("plot_benchmark_results.py", "r");
    if (test_py) {
        fclose(test_py);
        printf("Хотите сразу создать графики? (y/n): ");
        
        int answer = getchar();
        if (answer == 'y' || answer == 'Y') {
            printf("\nЗапуск Python скрипта...\n");
            #ifdef _WIN32
            system("python plot_benchmark_results.py");
            #else
            system("python3 plot_benchmark_results.py");
            #endif
        }
    } else {
        printf("Файл 'plot_benchmark_results.py' не найден.\n");
        printf("   Создайте его для визуализации результатов.\n");
    }
}