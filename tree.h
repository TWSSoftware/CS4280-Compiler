#include <stdlib.h>
#include "node.h"

struct node *buildTree(FILE *);
struct node *makeNode(struct node*, char *, int);
void traverseInorder(struct node *, FILE *);
void traversePreorder(struct node *, FILE *);
void traversePostorder(struct node *, FILE *);
void printNode(struct node *, int, FILE *);
