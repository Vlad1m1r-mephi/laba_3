.PHONY: all run run-file clean install debug valgrind format help
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

# Для Windows добавьте линковку с user32
ifeq ($(OS),Windows_NT)
    LDFLAGS = -luser32
    TARGET = program.exe
else
    LDFLAGS = 
    TARGET = program
endif
SOURCES = main.c app.c queue.c number_io.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = app.h queue.h number_io.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

run-file: $(TARGET)
	./$(TARGET) --file $(FILE)

benchmark: $(TARGET)
	@echo "🚀 Запуск автоматического бенчмарка..."
	./$(TARGET) --benchmark-auto

clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -rf benchmark_results/

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

debug: CFLAGS += -g -O0 -DDEBUG
debug: clean $(TARGET)

valgrind: debug
	valgrind --leak-check=full ./$(TARGET)

format:
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i




help:
	@echo "Доступные команды:"
	@echo "  make all       - Сборка программы"
	@echo "  make run       - Запуск программы"
	@echo "  make run-file  - Запуск с файлом (укажите FILE=имя_файла)"
	@echo "  make benchmark   - Запуск автоматического тестирования"
	@echo "  make clean     - Очистка проекта"
	@echo "  make debug     - Сборка для отладки"
	@echo "  make valgrind  - Проверка утечек памяти"
	@echo "  make format    - Форматирование кода"
	@echo "  make help      - Показать эту справку"