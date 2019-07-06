//Created By Philip Keroff

#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "token.h"
#include "scanner.h"
#include "parser.h"

struct token *lookTK;
static int statHelp = 0;

//error numbers for future use by the error handler
enum errorNum{SPEr, progVoidEr, blockStopEr, blockStartEr, varsIDEr, varsColEr,
	      varsIntEr, RRPAREr, REr, statEr, inIDEr, inDotEr, inScanEr, outLBREr,
	      outRBREr, outDotEr, outEr, ifLPAREr, ifRPAREr, ifEr, loopLPAREr, 
	      loopRPAREr, loopEr, letIDEr, letEQEr, letDotEr, letEr, ROEr}; 


struct node *startParser(FILE *input)
{
	struct node *root = malloc(sizeof(struct node));
	int level = 0;

	lookTK = malloc(sizeof(struct token));

	lookTK = scanner(input);

	//<program>
	root = progFunct(input, level);
	
	//If it reached the end of file without any errors
	if(lookTK->tkID == EOF_tk)
	{
		printf("Parser: The parse was successful!\n");
	}

	//Error
	else
	{
		error(SPEr, lookTK);
	}

	return root;
}

//<program> -> void <vars> <block>
struct node *progFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
	int currentChild = 0, currentTK = 0, counter;

	node->state = "program";
	node->level = level++;
	node->childNum = 0;
	node->tokenNum = 0;

	//set all the children to null
	for(counter = 0; counter < MAXST; counter++)
	{
		node->children[counter] = malloc(sizeof(struct node));
		node->children[counter] = NULL;
	}
	
	//set all the tokens to null
	for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//If void token
	if(lookTK->tkID == VOID_tk)
	{
		node->tokens[currentTK] = lookTK;

	 	lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<vars>
		node->children[currentChild] = varsFunct(input, level);
	
		//If the previous node didn't return null
                if(node->children[currentChild] != NULL)
                {
                        node->childNum++;
                        currentChild++;
                }

		//<stats>
		node->children[currentChild] = blockFunct(input, level);

		currentChild++;
                node->childNum++;
	}

	//error
	else
	{
		error(progVoidEr, lookTK);
	}

	return node;
}

//<block> -> start <vars> <stats> stop
struct node *blockFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
        int currentChild = 0, currentTK = 0, counter;

	node->state = "block";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//If start token
	if(lookTK->tkID == START_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<vars>
		node->children[currentChild] = varsFunct(input, level);

		//If the previous node didn't return null
                if(node->children[currentChild] != NULL)
                {
                        node->childNum++;
                        currentChild++;
                }

		//<stats>
		node->children[currentChild] = statsFunct(input, level);

		currentChild++;
                node->childNum++;

		//If not stop token
		if(lookTK->tkID != STOP_tk)
		{	
			error(blockStopEr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);

		node->tokenNum++;

		return node;
	}

	else
	{
		error(blockStartEr, lookTK);
	}

	return node;
}

//<vars> -> empty | var Identifier : Integer <vars>
struct node *varsFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
	int currentChild = 0, currentTK = 0, counter;

        node->state = "vars";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

	statHelp = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//If var token
	if(lookTK->tkID == VAR_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//If not Id token
		if(lookTK->tkID != IDENT_tk)
		{
			error(varsIDEr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//If not a colon token
		if(lookTK->tkID != COL_tk)
		{
			error(varsColEr, lookTK);
		} 

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//if not a number token
		if(lookTK->tkID != NUM_tk)
		{
			error(varsIntEr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<vars>
		node->children[currentChild] = varsFunct(input, level);

		//If the previous node didn't return null
		if(node->children[currentChild] != NULL)
		{
			node->childNum++;
			currentChild++;
		}

		return node;
	}

	//empty
	else
	{
		return NULL;
	}			
}

//<expr> -> <A> / <expr> | <A> * <expr> | <A>
struct node *exprFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
        int currentChild = 0, currentTK = 0, counter;

        node->state = "expr";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//<A>
	node->children[currentChild] = AFunct(input, level);

	currentChild++;
        node->childNum++;

	//if slash token
	if(lookTK->tkID == SLASH_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<expr>
		node->children[currentChild] = exprFunct(input, level);
		
                node->childNum++;

		return node;
	}

	//if star token
	else if(lookTK->tkID == STAR_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;
		
		//<expr>
		node->children[currentChild] = exprFunct(input, level);
		
                node->childNum++;
		
		return node;
	}

	//<A> by itself
	else
	{
		return node;
	}
}

//<A> -> <M> + <A> | <M> - <A> | <M>
struct node *AFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
        int currentChild = 0, currentTK = 0, counter;

        node->state = "A";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//<M>
	node->children[currentChild] = MFunct(input, level);

	currentChild++;
        node->childNum++;

	//if plus token
        if(lookTK->tkID == PLUS_tk)
        {
		node->tokens[currentTK] = lookTK;

                lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<expr>
                node->children[currentChild] = AFunct(input, level);
       
                node->childNum++;

		return node;
	 }

	//if minus token
        else if(lookTK->tkID == MIN_tk)
        {
		node->tokens[currentTK] = lookTK;

                lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<expr>
                node->children[currentChild] = AFunct(input, level);

		currentChild++;
                node->childNum++;
		
		return node;
        }

	//if <M> by itself
        else
        {
                return node;
        }
}

struct node *MFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
	int currentChild = 0, currentTK = 0, counter;

        node->state = "M";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//If minus token
	if(lookTK->tkID == MIN_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<M>
		node->children[currentChild] = MFunct(input, level);

                node->childNum++;
	
		return node;
	}

	//no minus
	else
	{
		//<R>
		node->children[currentChild] = RFunct(input, level);	

                node->childNum++;

		return node;
	}
}

//<R> -> ( <expr> ) | Identifier | Integer
struct node *RFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
        int currentChild = 0, currentTK = 0, counter;

        node->state = "R";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//if left parentheses
	if(lookTK->tkID == LPAR_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<expr>
		node->children[currentChild] = exprFunct(input, level);

		currentChild++;
                node->childNum++;

		//if not right parentheses
		if(lookTK->tkID != RPAR_tk)
		{
			error(RRPAREr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		return node;
	}

	//if identifier
	else if(lookTK->tkID == IDENT_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		return node;
	}

	//if integer
	else if(lookTK->tkID == NUM_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		return node;
	}

	//error
	else
	{
		error(REr, lookTK);
	}
}

//<stats> -> <stat> <mStat>
struct node *statsFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
	int currentChild = 0, currentTK = 0, counter;

        node->state = "stats";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//<stat>
	node->children[currentChild] = statFunct(input, level);

	currentChild++;
        node->childNum++;

	//<mStat>
	node->children[currentChild] = mStatFunct(input, level);

        //If the previous node didn't return null
        if(node->children[currentChild] != NULL)
        {
		node->childNum++;
		currentChild++;
        }

	return node;
}

//<mStat> -> empty | <stat> <mStat>
struct node *mStatFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
        int currentChild = 0, currentTK = 0, counter;

        node->state = "mStat";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//if it is a scan, out, start, if, loop, let token
	if(lookTK->tkID == SCAN_tk || lookTK->tkID == OUT_tk || lookTK->tkID == 
	   START_tk || lookTK->tkID == IF_tk || lookTK->tkID == LOOP_tk || 
	   lookTK->tkID == LET_tk)
	{
		//<stat>
		node->children[currentChild] = statFunct(input, level);

		currentChild++;
                node->childNum++;

		//<mStat>
		node->children[currentChild] = mStatFunct(input, level);

 		//If the previous node didn't return null
                if(node->children[currentChild] != NULL)
                {
                        node->childNum++;
                        currentChild++;
                }

		return node;
	}

	//empty
	else
	{
		return NULL;
	}
}

//<stat> -> <in> | <out> | <block> | <if> | <loop> | <assign>
struct node *statFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
        int currentChild = 0, currentTK = 0, counter;

        node->state = "stat";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//if scan token
	if(lookTK->tkID == SCAN_tk)
	{

		//To prevent vars to show up in errors
		statHelp = (ROEr + 1) - statEr;

		//<in>
		node->children[currentChild] = inFunct(input, level);

		currentChild++;
                node->childNum++;
	}

	//if out token
	else if(lookTK->tkID == OUT_tk)
	{
		//To prevent vars to show up in errors
                statHelp = (ROEr + 1) - statEr;

		//<out>
		node->children[currentChild] = outFunct(input, level);

		currentChild++;
                node->childNum++;
	}

	//if start token
	else if(lookTK->tkID == START_tk)
	{	
                //To prevent vars to show up in errors
                statHelp = (ROEr + 1) - statEr;

		//<block>
		node->children[currentChild] = blockFunct(input, level);
	
		currentChild++;
                node->childNum++;
	}

	//if if token
	else if(lookTK->tkID == IF_tk)
	{
                //To prevent vars to show up in errors
                statHelp = (ROEr + 1) - statEr;

		//<if>
		node->children[currentChild] = ifFunct(input, level);

		currentChild++;
                node->childNum++;
	}

	//if loop token
	else if(lookTK->tkID == LOOP_tk)
	{
                //To prevent vars to show up in errors
                statHelp = (ROEr + 1) - statEr;

		//<loop>
		node->children[currentChild] = loopFunct(input, level);

		currentChild++;
                node->childNum++;
	}

	//if let token
	else if(lookTK->tkID == LET_tk)
	{
                //To prevent vars to show up in errors
                statHelp = (ROEr + 1) - statEr;

		//<assign>
		node->children[currentChild] = assignFunct(input, level);

		currentChild++;
                node->childNum++;
	}

	//error
	else
	{
		error(statEr + statHelp, lookTK);
	}

	return node;
}

//<in> -> scan Identifier .
struct node *inFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
        int currentChild = 0, currentTK = 0, counter;

        node->state = "in";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//if scan token
	if(lookTK->tkID == SCAN_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//if not an identifier
		if(lookTK->tkID != IDENT_tk)
		{
			error(inIDEr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//if not dot token
		if(lookTK->tkID != DOT_tk)
		{
			error(inDotEr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;
	}

	//error
	else
	{
		error(inScanEr, lookTK);
	}

	return node;
}

//<out> -> out [ <expr ] .
struct node *outFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
        int currentChild = 0, currentTK = 0, counter;

        node->state = "out";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//if out token
	if(lookTK->tkID == OUT_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//if not left square brackets
		if(lookTK->tkID != LSQBRA_tk)
		{
			error(outLBREr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<expr>
		node->children[currentChild] = exprFunct(input, level);

		currentChild++;
                node->childNum++;

		//if not right square Brackets token
		if(lookTK->tkID != RSQBRA_tk)
		{
			error(outRBREr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;
		
		//if not dot token
		if(lookTK->tkID != DOT_tk)
		{
			error(outDotEr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		node->tokenNum++;
	}

	//error
	else
	{
		error(outEr, lookTK);
	}

	return node;
}

//<if> -> if ( <expr> <RO> <expr> ) <stat>
struct node *ifFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
        int currentChild = 0, currentTK = 0, counter;

        node->state = "if";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//if if token
	if(lookTK->tkID == IF_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;
		
		//if not left parentheses token
		if(lookTK->tkID != LPAR_tk)
		{
			error(ifLPAREr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<expr>
		node->children[currentChild] = exprFunct(input, level);

		currentChild++;
                node->childNum++;

		//<RO>
		node->children[currentChild] = ROFunct(input, level);

		currentChild++;
                node->childNum++;
		
		//<expr>
		node->children[currentChild] = exprFunct(input, level);

		currentChild++;
                node->childNum++;

		//if not right parentheses token
		if(lookTK->tkID != RPAR_tk)
		{
			error(ifRPAREr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<stat>
		node->children[currentChild] = statFunct(input, level);
	
		currentChild++;
                node->childNum++;
	}

	else
	{
		error(ifEr, lookTK);
	}

	return node;
}

//<loop> -> loop ( <expr > <RO> <expr> ) <stat>
struct node *loopFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
        int currentChild = 0, currentTK = 0, counter;

        node->state = "loop";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//if loop token
	if(lookTK->tkID == LOOP_tk)
        {
		node->tokens[currentTK] = lookTK;

                lookTK = scanner(input);
                
		currentTK++;
		node->tokenNum++;

		//if not left parentheses
                if(lookTK->tkID != LPAR_tk)
                {
                        error(loopLPAREr, lookTK);
                }

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
        	
		currentTK++;
		node->tokenNum++;

		//<expr>
                node->children[currentChild] = exprFunct(input, level);

		currentChild++;
                node->childNum++;
                
		//<RO>
                node->children[currentChild] = ROFunct(input, level);

		currentChild++;
                node->childNum++;
        
		//<expr>
                node->children[currentChild] = exprFunct(input, level);

		currentChild++;
                node->childNum++;

		//if not right paretheses
                if(lookTK->tkID != RPAR_tk)
                {
                        error(loopRPAREr, lookTK);
                }
        
		node->tokens[currentTK] = lookTK;

                lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<stat>
		node->children[currentChild] = statFunct(input, level);

                node->childNum++;
        }

	//error
        else
        {
                error(loopEr, lookTK);
        }

	return node;
}

//<assign> -> let Identifier = <expr> .
struct node *assignFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
        int currentChild = 0, currentTK = 0, counter;

        node->state = "assign";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
                node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//if let token
	if(lookTK->tkID == LET_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//if not identifier token
		if(lookTK->tkID != IDENT_tk)
		{
			error(letIDEr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//if not equal token
		if(lookTK->tkID != EQ_tk)
		{
			error(letEQEr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//<expr>	
		node->children[currentChild] = exprFunct(input, level);

                node->childNum++;

		//if not a dot token
		if(lookTK->tkID != DOT_tk)
		{
			error(letDotEr, lookTK);
		}

		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
	
		node->tokenNum++;
	}

	//error
	else
	{
		error(letEr, lookTK);
	}

	return node;
}

//<RO> -> < | < = | > | > = | = = | =
struct node *ROFunct(FILE *input, int level)
{
	struct node *node = malloc(sizeof(struct node));
        int currentChild = 0, currentTK = 0, counter;

        node->state = "RO";
        node->level = level++;
        node->childNum = 0;
        node->tokenNum = 0;

        //set all the children to null
        for(counter = 0; counter < MAXST; counter++)
        {
		node->children[counter] = malloc(sizeof(struct node));
		node->children[counter] = NULL;
        }

        //set all the tokens to null
        for(counter = 0; counter < MAXTK; counter++)
        {
		node->tokens[counter] = malloc(sizeof(struct token));
                node->tokens[counter] = NULL;
        }

	//if less than token
	if(lookTK->tkID == LESS_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//if equal token
		if(lookTK->tkID == EQ_tk)
		{
			node->tokens[currentTK] = lookTK;

			lookTK = scanner(input);
			
			currentTK++;
			node->tokenNum++;
		}
	}

	//if greater than token
	else if(lookTK->tkID == GREAT_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//if equal token
		if(lookTK->tkID == EQ_tk)
                {
			node->tokens[currentTK] = lookTK;

                        lookTK = scanner(input);
			
			currentTK++;
			node->tokenNum++;
                }
	}

	//if equal token
	else if(lookTK->tkID == EQ_tk)
	{
		node->tokens[currentTK] = lookTK;

		lookTK = scanner(input);
		
		currentTK++;
		node->tokenNum++;

		//if equal token
		if(lookTK->tkID == EQ_tk)
                {
			node->tokens[currentTK] = lookTK;

                        lookTK = scanner(input);
			
			node->tokenNum++;
                }
	}

	//error
	else
	{
		error(ROEr, lookTK);
	}

	return node;
}

//Error handler
void error(int errorNum, struct token *tk)
{
	//expected tokens
	char *correctTK[] = {"'eof'", "'void'", "'stop'", "'var' or 'start'", 
			"an Identifier Token", "':'", "aNumber Token", "')'", 
			"'(', an Identifier Token, or a Number Token",
			"'var', 'scan', 'out', 'start', 'if', 'loop', or 'let'", 
			"an Identifier Token", "'.'", "'scan'", "'['", 
			"']'", "'.'", "'out'", "'('", "')'", "'if'", "'('", "')'", 
			"'loop'", "an Identifier Token", "'='", "'.'", "'let'", 
			"'<', '>', or '='", 
			"'scan', 'out', 'start', 'if', 'loop', or 'let'"};
	
	printf("Parser: Error: Was not expecting %s at line %d!\n", tk->tokenInstance,
		tk->lineNum);

	printf("               Was expecting %s.\n", correctTK[errorNum]);

	exit(1);
}
