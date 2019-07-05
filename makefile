CC = gcc
CFLAGS = -c -o
DEPS = token.h node.h varStack.h scanner.h parser.h statSem.h
OBJ = main.c varStack.c scanner.c parser.c statSem.c

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $@ $<

comp : $(OBJ)
	$(CC) -o $@ $^
