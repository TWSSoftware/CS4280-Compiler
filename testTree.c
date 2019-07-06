//Created by Philip Keroff


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "testTree.h"

void testTree(struct node *treeNode)
{
	FILE *outFile;

	outFile = fopen("tree.data", "w");

	preOrderTrav(treeNode, outFile);
}

void preOrderTrav(struct node *treeNode, FILE *outFile)
{
	int nodeCount, tokenCount;

	printNode(treeNode, outFile);

	for(tokenCount = 0; tokenCount < treeNode->tokenNum; tokenCount++)
	{
		printToken(treeNode->tokens[tokenCount], outFile);
	}


	fprintf(outFile, "\n");

	for(nodeCount = 0; nodeCount < treeNode->childNum; nodeCount++)
	{
		preOrderTrav(treeNode->children[nodeCount], outFile);
	}
}

void printNode(struct node *treeNode, FILE *outFile)
{
	int counter;
	
	for(counter = 0; counter < treeNode->level; counter++)
	{
		fprintf(outFile, "  ");
	}

	fprintf(outFile, "%s", treeNode->state);
}

void printToken(struct token *token, FILE *outFile)
{
	fprintf(outFile, " %s", token->tokenInstance);
}
