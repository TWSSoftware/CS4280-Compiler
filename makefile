CC = gcc
CFLAGS = -c -o
DEPS = token.h scanner.h parser.h testTree.h
OBJ = main.c scanner.c parser.c testTree.c

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $@ $<

frontEnd : $(OBJ)
	$(CC) -o $@ $^
