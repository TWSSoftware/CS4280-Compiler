#ifndef NODE_H
#define NODE_H

#include "token.h"

#define MAXST 4
#define MAXTK 3

struct node
{
	char *state;
	int level, childNum, tokenNum;
	struct node *children[MAXST];
	struct token *tokens[MAXTK];
};

#endif
