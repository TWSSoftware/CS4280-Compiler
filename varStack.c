#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "token.h"
#include "varStack.h"

void push(struct token **stack, struct token *tk, int head)
{	
	stack[head] = tk;
	
	head++;
}

struct token *pop(struct token **stack, int head)
{
	struct token *retTK = malloc(sizeof(struct token));

	retTK = stack[head];
	
	stack[head] = NULL;

	head--;

	return retTK;
}

int find(struct token **stack, struct token *token, int head, int dif)
{
	int counter;

	for(counter = head - 1; counter >= dif; counter--)
	{
		if(strcmp(stack[counter]->tokenInstance, token->tokenInstance) == 0)
		{
			return counter;
		}
	} 

	return -1;
}
