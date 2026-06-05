CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lm -lpng

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=%.o)

decoder: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o decoder