#ifndef TESTTREE_H
#define TESTTREE_H

#include <stdlib.h>
#include "node.h"

void testTree(struct node *);
void preOrderTrav(struct node *, FILE *);
void printNode(struct node *, FILE *);
void printToken(struct token *, FILE *);

#endif
