//Created By Philip Keroff

#include <stdlib.h>
#include <stdio.h>
#include "token.h"
#include "testScanner.h"
#include "scanner.h"

//Driver Start Function
void testScanner(FILE *input)
{
	struct token *currentTK; 
	int counter;

	//Repeats until the EOF token is found
	while(currentTK->tkID != EOF_tk)
	{	
		currentTK = scanner(input);
		printTK(currentTK);
	}
}

//Prints a token
void printTK(struct token *currentTK)
{
	char *tkNames[] = {"Identifier", "start", "stop", "loop", "void", "var",
			   "return", "scan", "out", "program", "if", "then", "let", 
			   "=OP", "<OP", ">OP", ":OP", "+OP", "-OP", "*OP", "/OP",
			   "%OP", ".OP", "(OP", ")OP", ",OP", "{OP", "}OP", 
			   ";OP", "[OP", "]OP", "Integer", "EOF"};

	if(currentTK->tkID >= 1000)
	{
		printf("The current token, %s, is a %s, and resides on line %d\n", 
	       		currentTK->tokenInstance, tkNames[currentTK->tkID - 1000], 
	       		currentTK->lineNum);  
	}

	else
	{
		printf("hello\n");
	}
}

