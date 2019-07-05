#ifndef VARSTACK_H
#define VARSTACK_H

#include "token.h"

struct varStack
{
	int **tokNum;
	struct token **global;
	struct token **local;
};

void push(struct token **, struct token *, int);

struct token *pop(struct token **, int);

int find(struct token **, struct token *, int, int);

#endif
