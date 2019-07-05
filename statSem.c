//Created by Philip Keroff

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "statSem.h"
#include "varStack.h"

void statSem(struct node *treeNode, FILE *output)
{
	struct varStack *semStack;
	int *varLocNum = malloc(sizeof(int));
	int *varGlobNum = malloc(sizeof(int));
	int *scopeNum = malloc(sizeof(int));
	int counter;

	semStack = malloc(sizeof(struct varStack));

	//counts the number of global vars, local vars, and scopes
	preOrderTrav(treeNode, varGlobNum, varLocNum, scopeNum);

	semStack->global = malloc(sizeof(struct token*) * 100);
	semStack->local = malloc(sizeof(struct token*) * (*varLocNum));
	semStack->tokNum = malloc(sizeof(struct token*) * (*scopeNum));

	//Initializes the global stack for semStack
	for(counter = 0; counter < 100; counter++)
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

	codeGen(treeNode, semStack, output, 0);

	printf("Static Semantics: The program follows the proper static semantics!\n");

	fprintf(output, "STOP\n");
	
	printCons(semStack, output, *(semStack->tokNum[0]));
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
void codeGen(struct node *treeNode, struct varStack *stack, FILE *output, int scNum)
{
	struct token *tempTok = malloc(sizeof(struct token));
 	int nodeCount, tokenCount, dif, tempNum, location = 0;
	static int locCount = 0;
	char *label = malloc(sizeof(char) * 10);
	char *label2 = malloc(sizeof(char) * 10);
	bool rdChild = true;

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

			fprintf(output, "PUSH\n");
                }

		//If the identfier is not malready defined in the scope
                else if(!inLoc(stack, treeNode->tokens[1], locCount, dif))
                {
                        push(stack->local, treeNode->tokens[1], locCount);

                        *(stack->tokNum[scNum]) = *(stack->tokNum[scNum]) + 1;
			
			locCount++;

			fprintf(output, "PUSH\n");
                }

		//If the identifier is already defined in the current scope
                else
                {
                        errHand(1, treeNode->tokens[1]);
                }
	}

	else if(strcmp(treeNode->state, "expr") == 0)
	{
		rdChild = false;

		if(treeNode->tokenNum > 0)
		{
			codeGen(treeNode->children[1], stack, output, scNum);

			tempTok = tempVar();

			push(stack->global, tempTok, *(stack->tokNum[0]));

			*(stack->tokNum[0]) = *(stack->tokNum[0]) + 1;

			fprintf(output, "STORE %s\n", tempTok->tokenInstance);
		
			codeGen(treeNode->children[0], stack, output, scNum);

			if(strcmp(treeNode->tokens[0]->tokenInstance, "/") == 0)
			{
				fprintf(output, "DIV %s\n", tempTok->tokenInstance);
			}

			else
			{
				fprintf(output, "MULT %s\n", tempTok->tokenInstance);
			}
		}

		else
		{
			codeGen(treeNode->children[0], stack, output, scNum);
		}
	}

	else if(strcmp(treeNode->state, "A") == 0)
        {
                rdChild = false;

                if(treeNode->tokenNum > 0)
                {
                        codeGen(treeNode->children[1], stack, output, scNum);

                        tempTok = tempVar();

                        push(stack->global, tempTok, *(stack->tokNum[0]));

                        *(stack->tokNum[0]) = *(stack->tokNum[0]) + 1;

                        fprintf(output, "STORE %s\n", tempTok->tokenInstance);

                        codeGen(treeNode->children[0], stack, output, scNum);

                        if(strcmp(treeNode->tokens[0]->tokenInstance, "+") == 0)
                        {
                                fprintf(output, "ADD %s\n", tempTok->tokenInstance);
                        }

                        else
                        {
                                fprintf(output, "SUB %s\n", tempTok->tokenInstance);
                        }
                }

                else
                {
                        codeGen(treeNode->children[0], stack, output, scNum);
                }
        }

	else if(strcmp(treeNode->state, "M") == 0)
	{
		rdChild = false;

		if(strcmp(treeNode->children[0]->state, "R") == 0)
		{
			codeGen(treeNode->children[0], stack, output, scNum);
		}

		else
		{
			codeGen(treeNode->children[0], stack, output, scNum);
			fprintf(output, "MULT -1\n");
		}
	}

	else if(strcmp(treeNode->state, "R") == 0)
	{
		rdChild = false;

		if(treeNode->childNum > 0)
		{
			codeGen(treeNode->children[0], stack, output, scNum);
		}

		else
		{
			if((location = find(stack->local, treeNode->tokens[0], 
				locCount, 0)) != -1)
			{
				fprintf(output, "STACKR %d\n", location); 
			}
			
			else
			{
				fprintf(output, "LOAD %s\n", 
					treeNode->tokens[0]->tokenInstance);
			}
		}
	}

	else if(strcmp(treeNode->state, "in") == 0)
	{
		if((location = find(stack->local, treeNode->tokens[1], 
			locCount, 0)) != -1)
		{
			tempTok = tempVar();

			push(stack->global, tempTok, *(stack->tokNum[0]));

                        *(stack->tokNum[0]) = *(stack->tokNum[0]) + 1;

			fprintf(output, "READ %s\n", tempTok->tokenInstance);

			fprintf(output, "LOAD %s\n", tempTok->tokenInstance); 

			fprintf(output, "STACKW %d\n", location); 
		}

		else
		{
			fprintf(output, "READ %s\n", 
				treeNode->tokens[1]->tokenInstance);
		}
	}

	else if(strcmp(treeNode->state, "out") == 0)
	{	
		//prevents double reads
		rdChild = false;

		//calls child
		codeGen(treeNode->children[0], stack, output, scNum);

		//creates a new varable 
		tempTok = tempVar();
		
		//pushes the new variable to the global stack
		push(stack->global, tempTok,  *(stack->tokNum[0]));

		//Adds one to the global token number
		*(stack->tokNum[0]) = *(stack->tokNum[0]) + 1;

		//prints out the code for out
		fprintf(output, "STORE %s\n", tempTok->tokenInstance);
        	fprintf(output, "WRITE %s\n", tempTok->tokenInstance);
	}

	//Code gen for if
	else if(strcmp(treeNode->state, "if") == 0)
	{
		//Prevents double reads
		rdChild = false;
		
		//Calls third child
		codeGen(treeNode->children[2], stack, output, scNum);

		//Creates a new temparary variable
		tempTok = tempVar();

		push(stack->global, tempTok, *(stack->tokNum[0]));

		*(stack->tokNum[0]) = *(stack->tokNum[0]) + 1;

		//Stores value in tempVar
		fprintf(output, "STORE %s\n", tempTok->tokenInstance);

		//Calls first child
		codeGen(treeNode->children[0], stack, output, scNum);

		//Subtracts from tempVar
		fprintf(output, "SUB %s\n", tempTok->tokenInstance);

		//Jumps based on RO
		fprintf(output, "BR");

		//Prints out the correct jump from RO and returns the 
		//new label from the jump
		label = printRO(treeNode->children[1]->tokens, output, 
			treeNode->children[1]->tokenNum);
		
		//Calls the fourth child
		codeGen(treeNode->children[3], stack, output, scNum);

		//Prints the area it jumps to if it jumped
		fprintf(output, "%s: NOOP\n", label);
	}

	else if(strcmp(treeNode->state, "loop") == 0)
	{
		//Prevents double reads
		rdChild = false;

		//Creates label it jumps to at starts over the loop
		label2 = tempLab();

		fprintf(output, "%s: ", label2);
		
		codeGen(treeNode->children[2], stack, output, scNum);

		tempTok = tempVar();

		push(stack->global, tempTok, *(stack->tokNum[0]));
	
		*(stack->tokNum[0]) = *(stack->tokNum[0]) + 1;

		fprintf(output, "STORE %s\n", tempTok->tokenInstance);

		codeGen(treeNode->children[0], stack, output, scNum);

		fprintf(output, "SUB %s\n", tempTok->tokenInstance);

		fprintf(output, "BR");

		label = printRO(treeNode->children[1]->tokens, output,
			treeNode->children[1]->tokenNum);

		codeGen(treeNode->children[3], stack, output, scNum);

		fprintf(output, "BR %s\n", label2); 

		fprintf(output, "%s: NOOP\n", label);
	}

	else if(strcmp(treeNode->state, "assign") == 0)
	{
		//Prevents double reads of child
		rdChild = false;

		//calls child
		codeGen(treeNode->children[0], stack, output, scNum);

		//If it is local
		if((location = find(stack->local, treeNode->tokens[1], 
                        locCount, 0)) != -1)
                {
			fprintf(output, "STACKW %d\n", location);
		}

		//If it is global
		else
		{
			//Stores the value for the token
			fprintf(output, "STORE %s\n", 
				treeNode->tokens[1]->tokenInstance);
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
			codeGen(treeNode->children[nodeCount], stack, output, 
				(scNum + 1));
	
			//If it is the end of the block node
			if((nodeCount + 1) == treeNode->childNum)
			{
				tempNum = locCount - 1;
	
				locCount -= *(stack->tokNum[scNum + 1]);

				delBlkVars(stack, (scNum + 1), tempNum, output);
			}
		}
		
		//If the current node is not in the block state
		else if(rdChild)
		{
                	codeGen(treeNode->children[nodeCount], stack, output, scNum);
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
void delBlkVars(struct varStack *stack, int scNum, int locCount, FILE *output)
{
	struct token *tempTok = malloc(sizeof(struct token));

	//Repeats until there are not more variables left from the block
	while(*(stack->tokNum[scNum]) > 0)
	{
		tempTok = pop(stack->local, locCount);

		*(stack->tokNum[scNum]) = *(stack->tokNum[scNum]) - 1;

		locCount--;

		fprintf(output, "POP\n");
	}
}

struct token *tempVar()
{
	struct token *tempToken = malloc(sizeof(struct token));
	static int countVar = 0;

	tempToken->tkID = IDENT_tk;
	tempToken->lineNum = -1;

	tempToken->tokenInstance = malloc(sizeof(char) * 10);

	sprintf(tempToken->tokenInstance, "TempV%d", countVar);

	countVar++;

	return tempToken;
}

char *tempLab()
{
	static int countLab = 0;
	char *tempLabel = malloc(sizeof(char) * 10);

	sprintf(tempLabel, "TempL%d", countLab);

	countLab++;

	return tempLabel;
}

void printCons(struct varStack *stack, FILE *output, int max)
{
	int counter;

	for(counter = 0; counter < max; counter++)
	{
		fprintf(output, "%s 0\n", stack->global[counter]->tokenInstance);
	}
}

char *printRO(struct token **tokens, FILE *output, int max)
{
	int counter;
	char *tempLabel;
	char *RO = malloc(sizeof(char) * 2);

	tempLabel = tempLab();

	for(counter = 0; counter < max; counter++)
	{
		strcat(RO, tokens[counter]->tokenInstance);
	}

	if(strcmp(RO, "<") == 0)
	{
		fprintf(output, "ZPOS %s\n", tempLabel);
	}

	else if(strcmp(RO, "<=" ) == 0)
	{
		fprintf(output, "POS %s\n", tempLabel);
	}

	else if(strcmp(RO, ">") == 0)
        {
                fprintf(output, "ZNEG %s\n", tempLabel);
        }

	else if(strcmp(RO, ">=") == 0)
        {
                fprintf(output, "NEG %s\n", tempLabel);
        }

	else if(strcmp(RO, "==") == 0)
        {
                fprintf(output, "ZERO %s\n", tempLabel);
        }

	else
	{
		fprintf(output, "NEG %s\n", tempLabel);
		fprintf(output, "BRPOS %s\n", tempLabel);
	}

	return tempLabel;
}
