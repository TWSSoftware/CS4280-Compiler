#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "tree.h"

//Creates a tree from the file
struct node *buildTree(FILE *input)
{
	int counter;
	char tempWord[200];
	struct node* root = NULL;

	//Takes a word up till a space and creates a node from the word
	while(fscanf(input, "%s", tempWord) != EOF)
	{
		for(counter = 0; counter < strlen(tempWord); counter++)
		{
			tempWord[counter] = tolower(tempWord[counter]);
		}

		root = makeNode(root, tempWord, 0);
	}
		
	return root;
}

//Creates a node for the tree
struct node *makeNode(struct node* node, char *word, int level)
{
	int tempLev = level;

	//If the current Node is Null
        if(node == NULL)
        {
                node = malloc(sizeof(struct node));
		node->word = malloc(sizeof(char) * strlen(word));
                strcpy(node->word, word);
                node->level = level;

                node->leftChild = NULL;
                node->rightChild = NULL;
		node->twin = NULL;
        }

	//If the current node is not Null
        else
        {
		//If the first character is less than the previous
                if(node->word[0] > word[0])
                {
                        tempLev++;

                        node->leftChild = makeNode(node->leftChild, word, tempLev);
                }

		//If the first character is greater than the previous
                else if(node->word[0] < word[0])
                {
                        tempLev++;

                        node->rightChild = makeNode(node->rightChild, word, tempLev);
                }

		//If they have the same first character
                else
                {
			//If the word equals another word
			if(strcmp(node->word, word) == 0)
			{
				printf("Word was already there!\n");
			}
			
			//If the word doesn't equal another word
			else
			{
				node->twin = makeNode(node->twin, word, tempLev);
			}
                }
        }

        return node;
}

//Inorder Traversal
void traverseInorder(struct node *node, FILE *output)
{
	if(node != NULL)
	{
		traverseInorder(node->leftChild, output);
		printNode(node, 0, output);
		traverseInorder(node->rightChild, output);
	}
}

//Preorder Traversal
void traversePreorder(struct node *node, FILE *output)
{
	if(node != NULL)
	{
		printNode(node, 0, output);
		traversePreorder(node->leftChild, output);
		traversePreorder(node->rightChild, output);
	}
}

//Postorder Traversal
void traversePostorder(struct node *node, FILE *output)
{
	if(node != NULL)
        {
                traversePostorder(node->leftChild, output);
                traversePostorder(node->rightChild, output);
		printNode(node, 0, output);
        }

}

//Prints the nodes to a file
void printNode(struct node *node, int flag, FILE *output)
{
	int counter;

	//If it is the first word of the twin's linked list
	if(flag == 0)
	{
		for(counter = 0; counter < node->level; counter++)
		{
			fprintf(output, "  ");
		}
	}

	fprintf(output, "%s ", node->word);

	//If the node has another twin on the linked list
	if(node->twin != NULL)
	{
		printNode(node->twin, 1, output);
	}

	//If is is the first word of the twin's linked list
	if(flag == 0)
	{
		fprintf(output, "\n");
	}
}
