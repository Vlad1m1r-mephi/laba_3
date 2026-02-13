.PHONY: all run run-file clean help
# FILE = data.txt

TARGET = program
SOURCES = main.c app.c queue.c number_io.c	
#OBJECTS = main.o app.o number_io.o queue.o

all:
	gcc $(SOURCES) -o $(TARGET)
	
run: $(TARGET)
#gcc $(SOURCES) -o $(TARGET)
	./$(TARGET)

benchmark: $(TARGET)
	@echo "Запуск автоматического бенчмарка..."
	./$(TARGET) --benchmark-auto

clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -rf benchmark_results/

help:
	@echo "Доступные команды:"
	@echo "  make all       - Сборка программы"
	@echo "  make run       - Запуск программы"
	@echo "  make benchmark   - Запуск автоматического тестирования"
	@echo "  make clean     - Очистка проекта"	
	@echo "  make help      - Показать эту справку"