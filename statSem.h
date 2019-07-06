#ifndef TESTTREE_H
#define TESTTREE_H

#include <stdlib.h>
#include <stdbool.h>
#include "node.h"
#include "varStack.h"

void statSem(struct node *);
void preOrderTrav(struct node *, int *, int *, int *);
void chkSem(struct node *, struct varStack *, int);
bool inGlob(struct varStack *, struct token *, int);
bool inLoc(struct varStack *, struct token *, int, int);
void errHand(int, struct token *);
void delBlkVars(struct varStack *, int, int);

#endif
