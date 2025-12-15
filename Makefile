CC     = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2

OBJ = main.o app.o queue.o number_io.o sort.o

all: lab3

lab3: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) lab3
