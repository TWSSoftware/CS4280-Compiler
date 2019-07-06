//Created by Philip Keroff

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "statSem.h"
#include "varStack.h"

void statSem(struct node *treeNode)
{
	struct varStack *semStack;
	int *varLocNum = malloc(sizeof(int));
	int *varGlobNum = malloc(sizeof(int));
	int *scopeNum = malloc(sizeof(int));
	int counter;

	semStack = malloc(sizeof(struct varStack));

	//counts the number of global vars, local vars, and scopes
	preOrderTrav(treeNode, varGlobNum, varLocNum, scopeNum);

	semStack->global = malloc(sizeof(struct token*) * (*varGlobNum));
	semStack->local = malloc(sizeof(struct token*) * (*varLocNum));
	semStack->tokNum = malloc(sizeof(struct token*) * (*scopeNum));

	//Initializes the global stack for semStack
	for(counter = 0; counter < *varGlobNum; counter++)
	{
		semStack->global[counter] = malloc(sizeof(struct token));
	}

	//Initializes the local stack for semStack
	for(counter = 0; counter < *varLocNum; counter++)
	{
		semStack->local[counter] = malloc(sizeof(struct token));
	}

	//Initializes the tokNum for semStack
	for(counter = 0; counter < *scopeNum; counter++)
	{
		semStack->tokNum[counter] = malloc(sizeof(int));
		*(semStack->tokNum[counter]) = 0;
	}

	chkSem(treeNode, semStack, 0);

	printf("Static Semantics: The program follows the proper static semantics!\n");
}

//Used to count the number of vars for the global and local stacks and the number of
//scopes
void preOrderTrav(struct node *treeNode, int *varGNum, int *varLNum, int *scopeNum)
{
	int nodeCount;
	static bool global = true;
	static int varLocCount = 0, varGlobCount = 0, scopeCount = 1;

	//If the node's state is vars and global
	if(strcmp(treeNode->state, "vars") == 0 && global)
	{
		varGlobCount++;
	}

	//If the node's state is vars and not global
	else if(strcmp(treeNode->state, "vars") == 0 && !global) 
	{
		varLocCount++;
	}

	//If the node's state is block
	else if(strcmp(treeNode->state, "block") == 0)
	{
		//variables are no longer global
		global = false;

		scopeCount++;
	}

	for(nodeCount = 0; nodeCount < treeNode->childNum; nodeCount++)
	{
		preOrderTrav(treeNode->children[nodeCount], varGNum, varLNum,scopeNum);
	}

	*varGNum = varGlobCount;

	*varLNum = varLocCount;
	
	*scopeNum = scopeCount;
}

//Checks for the static semantics for the program
void chkSem(struct node *treeNode, struct varStack *stack, int scNum)
{
 	int nodeCount, tokenCount, dif, tempNum;
	static int locCount = 0;

	//If it is global
        if(strcmp(treeNode->state, "vars") == 0 && scNum == 0)
        {
		//If it is the first identifier
		if(*(stack->tokNum[scNum]) == 0)
		{
			push(stack->global, treeNode->tokens[1], 
			     *(stack->tokNum[scNum]));

			*(stack->tokNum[scNum]) = *(stack->tokNum[scNum]) + 1;
		}

		//If the identifier is not defined
		else if(!inGlob(stack, treeNode->tokens[1], *(stack->tokNum[scNum])))
		{
         		push(stack->global, treeNode->tokens[1],
			     *(stack->tokNum[scNum])); 
			
			*(stack->tokNum[scNum]) = *(stack->tokNum[scNum]) + 1;
		}

		//If the token is already defined as a global variable
		else
		{
			errHand(1, treeNode->tokens[1]);	
		}	
        }

	//If it is in the local scope
	else if(strcmp(treeNode->state, "vars") == 0 && scNum != 0)
	{
		dif = locCount - *(stack->tokNum[scNum]);

		//If it is the first token in the scope
		if(*(stack->tokNum[scNum]) == 0)
                {
                        push(stack->local, treeNode->tokens[1], locCount);

                        *(stack->tokNum[scNum]) = *(stack->tokNum[scNum]) + 1;

			locCount++;
                }

		//If the identfier is not already defined in the scope
                else if(!inLoc(stack, treeNode->tokens[1], locCount, dif))
                {
                        push(stack->local, treeNode->tokens[1], locCount);

                        *(stack->tokNum[scNum]) = *(stack->tokNum[scNum]) + 1;
			
			locCount++;
                }

		//If the identifier is already defined in the current scope
                else
                {
                        errHand(1, treeNode->tokens[1]);
                }
	}

	//Checks the tokens for each state for identifier tokens
	for(tokenCount = 0; tokenCount < treeNode->tokenNum; tokenCount++)
	{
		//If the current node has a identifier token and the state is not vars
		if(treeNode->tokens[tokenCount]->tkID == IDENT_tk && 
		   strcmp(treeNode->state, "vars"))
		{
			//If the identifier used is not in the local or global stack
			if(!(inLoc(stack, treeNode->tokens[tokenCount], locCount, 0) ||
			   inGlob(stack, treeNode->tokens[tokenCount], 
				  *(stack->tokNum[0]))))
			{
				errHand(2, treeNode->tokens[tokenCount]);
			}
		}
	}

	//Repeats until there are no more children and changes scope during block state
        for(nodeCount = 0; nodeCount < treeNode->childNum; nodeCount++)
        {
		//If the current node is in the block state
		if(strcmp(treeNode->state, "block") == 0)
		{
			chkSem(treeNode->children[nodeCount], stack, (scNum + 1));
	
			//If it is the end of the block node
			if((nodeCount + 1) == treeNode->childNum)
			{
				tempNum = locCount - 1;
	
				locCount -= *(stack->tokNum[scNum + 1]);

				delBlkVars(stack, (scNum + 1), tempNum);
			}
		}
		
		//If the current node is not in the block state
		else
		{
                	chkSem(treeNode->children[nodeCount], stack, scNum);
		}
        }
}

//Checks if the variable is in the global stack
bool inGlob(struct varStack *stack, struct token *nextTK, int max)
{
	int loc;
	
	loc = find(stack->global, nextTK, max, 0);
	
	//If it found it
	if(loc != -1)
	{
		return true;
	}

	//If it didn't find it
	else
	{
		return false;
	}
}

//Checks if it is in the local scope
bool inLoc(struct varStack *stack, struct token *nextTK, int max, int dif)
{
        int loc;

        loc = find(stack->local, nextTK, max, dif); 

	//If it did find it
        if(loc != -1)
	{
		return true;
	}

	//If it didn't find it
	else
	{
		return false;
	}
}

//Error handler 
void errHand(int errNum, struct token *errTK)
{
	printf("Static Semantics: Error: ");

	//Already defined error
	if(errNum == 1)
	{
		printf("%s on line %d was already defined!\n", errTK->tokenInstance,
			errTK->lineNum);
	}

	//Not defined error
	else if(errNum == 2)
	{
		printf("%s was not defined before it was used on line %d!\n",
			errTK->tokenInstance, errTK->lineNum);
	}

	exit(1);
}

//Removes the variables within the block
void delBlkVars(struct varStack *stack, int scNum, int locCount)
{
	struct token *tempTok = malloc(sizeof(struct token));

	//Repeats until there are not more variables left from the block
	while(*(stack->tokNum[scNum]) > 0)
	{
		tempTok = pop(stack->local, locCount);

		*(stack->tokNum[scNum]) = *(stack->tokNum[scNum]) - 1;

		locCount--;
	}
}
