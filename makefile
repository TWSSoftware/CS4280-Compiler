CC = gcc
CFLAGS = -c -o
DEPS = token.h scanner.h testScanner.h
OBJ = main.c scanner.c testScanner.c

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $@ $<

P1: $(OBJ)
	$(CC) -o $@ $^
