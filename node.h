#ifndef NODE_H
#define NODE_H

struct node
{
	char *word;
	int level;
	struct node *leftChild;
	struct node *rightChild;
	struct node *twin;
};

#endif
