#ifndef TESTTREE_H
#define TESTTREE_H

#include <stdlib.h>
#include <stdbool.h>
#include "node.h"
#include "varStack.h"

void statSem(struct node *, FILE *);
void preOrderTrav(struct node *, int *, int *, int *);
void codeGen(struct node *, struct varStack *, FILE *, int);
bool inGlob(struct varStack *, struct token *, int);
bool inLoc(struct varStack *, struct token *, int, int);
void errHand(int, struct token *);
void delBlkVars(struct varStack *, int, int, FILE *);
struct token *tempVar();
char *tempLab();
void printCons(struct varStack *, FILE *, int);
char *printRO(struct token **, FILE * , int);


#endif
