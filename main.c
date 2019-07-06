#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "tree.h"

//main function
int main(int argc, char *argv[])
{
	struct node *root = malloc(sizeof(struct node));
	char *inFileName = NULL;
	char *outFileIn, *outFilePre, *outFilePost, tempWords[200];
	FILE *input, *outIn, *outPre, *outPost, *tempfile;
	int fileCheck = 0;

	//Error more than three arguments
	if(argc >= 3)
	{
		printf("Error: two many arguments\n");

		return -1;
	}

	//If there is an argument for a file
	else if(argc == 2)
	{
		inFileName = malloc(sizeof(char) * (strlen(argv[1]) + 6));
		outFileIn = malloc(sizeof(char) * (strlen(argv[1]) + 8));
		outFilePre = malloc(sizeof(char) * (strlen(argv[1]) + 9));
		outFilePost = malloc(sizeof(char) * (strlen(argv[1]) + 10));
		
		strcpy(inFileName, argv[1]);
		strcpy(outFileIn, inFileName);
		strcpy(outFilePre, inFileName);
		strcpy(outFilePost, inFileName);

		strcat(inFileName, ".fs182");
		strcat(outFileIn, ".inorder");
		strcat(outFilePre, ".preorder");
		strcat(outFilePost, ".postorder");
		
		input = fopen(inFileName, "r");
		
		if(input == NULL)
		{
			printf("Error: That file doesn't Exist\n");
			return 1;
		}
	}

	//If it is standard Input
	else
	{	
		fileCheck = 1;

		//Opens a tempfile to write stdin
		tempfile = fopen("tempfile", "w");

		//Scans stdin until EOF
		while(scanf("%s", tempWords) != EOF)
		{
			fprintf(tempfile, "%s ", tempWords);
		}
		
		//Closes the writable tempfile
		fclose(tempfile);

		tempfile = fopen("tempfile", "r");

		//If tempfile failed to be created
		if(tempfile == NULL)
		{
			printf("Error: tempfile was not created");
			return 1;
		}	
		
		//Sets input to tempfile creating the tree
		input = tempfile;
	}

	//If there was an argument file
	if(inFileName != NULL)
	{
		outIn = fopen(outFileIn, "w");
		outPre = fopen(outFilePre, "w");
		outPost = fopen(outFilePost, "w");
	}

	//If it is standard input
	else
	{
		outIn = fopen("out.inorder", "w");
                outPre = fopen("out.preorder", "w");
                outPost = fopen("out.postorder", "w");
	}

	//Builds the tree
	root = buildTree(input);

	//Inorder Traversal
	traverseInorder(root, outIn);

	//Preorder Traversal
	traversePreorder(root, outPre);

	//Postorder Traversal
	traversePostorder(root, outPost);

	//close the files
	fclose(input);
	fclose(outIn);
	fclose(outPre); 
	fclose(outPost); 

	//deletes the tempfile
	if(fileCheck == 1)
	{
		remove("tempfile");
	}

	return 0;
}
