//Created By Philip Keroff

#ifndef SCANNER_H
#define SCANNER_H

#include <stdlib.h>
#include "token.h"

struct token *scanner(FILE *);
void printError(int, int, char);

#endif

