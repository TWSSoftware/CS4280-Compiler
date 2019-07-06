CC = gcc
CFLAGS = -c -o
DEPS = node.h tree.h
OBJ = main.c tree.c

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $@ $<

P0: $(OBJ)
	$(CC) -o $@ $^

clear:
	rm *.inorder *.preorder *.postorder
