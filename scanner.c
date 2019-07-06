//Created By Philip Keroff

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "token.h"
#include "scanner.h"

#define MAXLEN 8
#define LAROW 36
#define LACOL 26

//State Values
enum state{S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15,
	   S16, S17, S18, S19, S20, S21, S22, S23, S24, S25, S26, S27, S28, 
	   S29, S30, S31, S32, S33, S34, S35, S36};

//Charactor Values
enum charChk{LU, LL, D, _, WS, EQ, LT, GT, COL, PLUS, MIN, STAR, SLSH, PER, 
	     DOT, LP, RP, COM, LC, RC, SEM, LS, RS, EL, END, $};

//Error Values
enum error{E1 = -1, E2 = -2, E3 = -3};

//Scanner Function
struct token *scanner(FILE *input)
{
	struct token *presToken = malloc(sizeof(struct token));
	static char lookahead, curChar;
	int counter, rowCount, colCount, charEl, nextChar = 0, curState = S1;
	int errNum, prevState = S2, repeat = 1, comStart;
	static int lineCount = 1, initial = 1, comment = 0;

	//FSA table
	static int lexAn[LAROW][LACOL] = {
		{E1, S2, S28, E1, S1, S10, S11, S12, S13, S14, S15, S16, S17, S18,
		  S19, S20, S21, S22, S23, S24, S25, S26, S27, E2, EOF_tk, S36},
		{S3, S3, S3, S3, IDENT_tk, IDENT_tk, IDENT_tk,  IDENT_tk, IDENT_tk,
		   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk,
		   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk,
		   IDENT_tk, IDENT_tk, IDENT_tk, S36},
		{S4, S4, S4, S4, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, 
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, S36},
		{S5, S5, S5, S5, IDENT_tk, IDENT_tk, IDENT_tk,  IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, 
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, S36},
		{S6, S6, S6, S6, IDENT_tk, IDENT_tk, IDENT_tk,  IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, 
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, S36},
		{S7, S7, S7, S7, IDENT_tk, IDENT_tk, IDENT_tk,  IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, 
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, S36},
		{S8, S8, S8, S8, IDENT_tk, IDENT_tk, IDENT_tk,  IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, S36},
		{S9, S9, S9, S9, IDENT_tk, IDENT_tk, IDENT_tk,  IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk,
                   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, S36},
		{IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, 
		   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, 
		   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, 
		   IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk, IDENT_tk,
		   IDENT_tk, S36},
		{EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk,
		   EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk,
		   EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk, EQ_tk, S36},
		{LESS_tk, LESS_tk, LESS_tk, LESS_tk, LESS_tk, LESS_tk, LESS_tk,
		   LESS_tk, LESS_tk, LESS_tk, LESS_tk, LESS_tk, LESS_tk, LESS_tk,
		   LESS_tk, LESS_tk, LESS_tk, LESS_tk, LESS_tk, LESS_tk, LESS_tk,
		   LESS_tk, LESS_tk, LESS_tk, LESS_tk, S36},
		{GREAT_tk, GREAT_tk, GREAT_tk, GREAT_tk, GREAT_tk, GREAT_tk, 
		   GREAT_tk, GREAT_tk, GREAT_tk, GREAT_tk, GREAT_tk, GREAT_tk,
		   GREAT_tk, GREAT_tk, GREAT_tk, GREAT_tk, GREAT_tk, GREAT_tk,
		   GREAT_tk, GREAT_tk, GREAT_tk, GREAT_tk, GREAT_tk, GREAT_tk,
		   GREAT_tk, S36},
		{COL_tk, COL_tk, COL_tk, COL_tk, COL_tk, COL_tk, COL_tk, COL_tk,
		   COL_tk, COL_tk, COL_tk, COL_tk, COL_tk, COL_tk, COL_tk, COL_tk,
		   COL_tk, COL_tk, COL_tk, COL_tk, COL_tk, COL_tk, COL_tk, COL_tk,
		   COL_tk, S36},
		{PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk,
		   PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk,
		   PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk,
		   PLUS_tk, PLUS_tk, PLUS_tk, PLUS_tk, S36},
		{MIN_tk, MIN_tk, MIN_tk, MIN_tk, MIN_tk, MIN_tk, MIN_tk, MIN_tk,
		   MIN_tk, MIN_tk, MIN_tk, MIN_tk, MIN_tk, MIN_tk, MIN_tk, MIN_tk,
		   MIN_tk, MIN_tk, MIN_tk, MIN_tk, MIN_tk, MIN_tk, MIN_tk, MIN_tk,
		   MIN_tk, S36},
		{STAR_tk, STAR_tk, STAR_tk, STAR_tk, STAR_tk, STAR_tk, STAR_tk,
		   STAR_tk, STAR_tk, STAR_tk, STAR_tk, STAR_tk, STAR_tk, STAR_tk,
		   STAR_tk, STAR_tk, STAR_tk, STAR_tk, STAR_tk, STAR_tk, STAR_tk,
		   STAR_tk, STAR_tk, STAR_tk, STAR_tk, S36},
		{SLASH_tk, SLASH_tk, SLASH_tk, SLASH_tk, SLASH_tk, SLASH_tk,
		   SLASH_tk, SLASH_tk, SLASH_tk, SLASH_tk, SLASH_tk, SLASH_tk,
		   SLASH_tk, SLASH_tk, SLASH_tk, SLASH_tk, SLASH_tk, SLASH_tk,
		   SLASH_tk, SLASH_tk, SLASH_tk, SLASH_tk, SLASH_tk, SLASH_tk,
		   SLASH_tk, S36},
		{PER_tk, PER_tk, PER_tk, PER_tk, PER_tk, PER_tk, PER_tk, PER_tk,
		   PER_tk, PER_tk, PER_tk, PER_tk, PER_tk, PER_tk, PER_tk, PER_tk,
		   PER_tk, PER_tk, PER_tk, PER_tk, PER_tk, PER_tk, PER_tk, PER_tk,
		   PER_tk, S36},
		{DOT_tk, DOT_tk, DOT_tk, DOT_tk, DOT_tk, DOT_tk, DOT_tk, DOT_tk,
		   DOT_tk, DOT_tk, DOT_tk, DOT_tk, DOT_tk, DOT_tk, DOT_tk, DOT_tk,
		   DOT_tk, DOT_tk, DOT_tk, DOT_tk, DOT_tk, DOT_tk, DOT_tk, DOT_tk,
		   DOT_tk, S36},
		{LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk,
		   LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk,
		   LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk,
		   LPAR_tk, LPAR_tk, LPAR_tk, LPAR_tk, S36},
		{RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk,
		   RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk,
		   RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk,
		   RPAR_tk, RPAR_tk, RPAR_tk, RPAR_tk, S36},
		{COM_tk, COM_tk, COM_tk, COM_tk, COM_tk, COM_tk, COM_tk, COM_tk,
		   COM_tk, COM_tk, COM_tk, COM_tk, COM_tk, COM_tk, COM_tk, COM_tk,
		   COM_tk, COM_tk, COM_tk, COM_tk, COM_tk, COM_tk, COM_tk, COM_tk,
		   COM_tk, S36},
		{LCUBRA_tk, LCUBRA_tk, LCUBRA_tk, LCUBRA_tk, LCUBRA_tk, LCUBRA_tk,
		   LCUBRA_tk, LCUBRA_tk, LCUBRA_tk, LCUBRA_tk, LCUBRA_tk, LCUBRA_tk,
		   LCUBRA_tk, LCUBRA_tk, LCUBRA_tk, LCUBRA_tk, LCUBRA_tk, LCUBRA_tk,
		   LCUBRA_tk, LCUBRA_tk, LCUBRA_tk, LCUBRA_tk, LCUBRA_tk, LCUBRA_tk,
		   LCUBRA_tk, S36},
		{RCUBRA_tk, RCUBRA_tk, RCUBRA_tk, RCUBRA_tk, RCUBRA_tk, RCUBRA_tk,
		   RCUBRA_tk, RCUBRA_tk, RCUBRA_tk, RCUBRA_tk, RCUBRA_tk, RCUBRA_tk,
		   RCUBRA_tk, RCUBRA_tk, RCUBRA_tk, RCUBRA_tk, RCUBRA_tk, RCUBRA_tk,
		   RCUBRA_tk, RCUBRA_tk, RCUBRA_tk, RCUBRA_tk, RCUBRA_tk, RCUBRA_tk,
		   RCUBRA_tk, S36},
		{SEMCOL_tk, SEMCOL_tk, SEMCOL_tk, SEMCOL_tk, SEMCOL_tk, SEMCOL_tk,
		   SEMCOL_tk, SEMCOL_tk, SEMCOL_tk, SEMCOL_tk, SEMCOL_tk, SEMCOL_tk,
		   SEMCOL_tk, SEMCOL_tk, SEMCOL_tk, SEMCOL_tk, SEMCOL_tk, SEMCOL_tk,
		   SEMCOL_tk, SEMCOL_tk, SEMCOL_tk, SEMCOL_tk, SEMCOL_tk, SEMCOL_tk,
		   SEMCOL_tk, S36},
		{LSQBRA_tk, LSQBRA_tk, LSQBRA_tk, LSQBRA_tk, LSQBRA_tk, LSQBRA_tk,
		   LSQBRA_tk, LSQBRA_tk, LSQBRA_tk, LSQBRA_tk, LSQBRA_tk, LSQBRA_tk,
		   LSQBRA_tk, LSQBRA_tk, LSQBRA_tk, LSQBRA_tk, LSQBRA_tk, LSQBRA_tk,
		   LSQBRA_tk, LSQBRA_tk, LSQBRA_tk, LSQBRA_tk, LSQBRA_tk, LSQBRA_tk,
		   LSQBRA_tk, S36},
		{RSQBRA_tk, RSQBRA_tk, RSQBRA_tk, RSQBRA_tk, RSQBRA_tk, RSQBRA_tk,
		   RSQBRA_tk, RSQBRA_tk, RSQBRA_tk, RSQBRA_tk, RSQBRA_tk, RSQBRA_tk,
		   RSQBRA_tk, RSQBRA_tk, RSQBRA_tk, RSQBRA_tk, RSQBRA_tk, RSQBRA_tk,
		   RSQBRA_tk, RSQBRA_tk, RSQBRA_tk, RSQBRA_tk, RSQBRA_tk, RSQBRA_tk,
		   RSQBRA_tk, S36},
		{NUM_tk, NUM_tk, S29, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
		   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
		   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
		   NUM_tk, S36},
		{NUM_tk, NUM_tk, S30, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
		   NUM_tk, S36},
		{NUM_tk, NUM_tk, S31, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
		   NUM_tk, S36},
		{NUM_tk, NUM_tk, S32, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
		   NUM_tk, S36},
		{NUM_tk, NUM_tk, S33, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
		   NUM_tk, S36},
		{NUM_tk, NUM_tk, S34, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
		   NUM_tk, S36},
		{NUM_tk, NUM_tk, S35, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
		   NUM_tk, S36},
		{NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
                   NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk, NUM_tk,
		   NUM_tk, S36},
		{S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36,
		   S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, E3, S1}};
	
	//If it is the first time running the program	
	if(initial == 1)
        {	
/*Prints out the Table
		printf("The FSA table for this Compiler is:\n");


		for(rowCount = 0; rowCount < LAROW; rowCount++)
		{
			for(colCount = 0; colCount < LACOL; colCount++)
			{
				printf("|%4d", lexAn[rowCount][colCount]);
			}
		
			printf("\n");
		}
*/
		
		initial = 0;

		curChar = fgetc(input);

		lookahead = fgetc(input);
	}

	presToken->tokenInstance = malloc(sizeof(char) * MAXLEN);

	//Repeats until the max length has been met or until it finds the token
	for(counter = 0; counter <= MAXLEN && repeat != 0; counter++)
	{
		presToken->lineNum = lineCount;
		
		//If it is a part of a comment	
		if(curChar == '$' || comment == 1)
                {
                        counter = 0;

                        charEl = $;

			if(curChar == '\n')
			{
				lineCount++;
			}
			
			//If it is end of the comment
                        if(curChar == '$' && comment == 1)
                        {
                                comment = 0;
                        }
                    
			//If it is the start of the comment
                        else if(curChar == '$' && comment != 1)
                        {
                                prevState = curState;

                                comment = 1;

				comStart =lineCount;
                        }

			//If the file reached the EOF without finnishing 
			//the comment
                        else if(curChar == EOF)
                        {
				curState = S36;

                                charEl = END;

				strcat(presToken->tokenInstance, "EOFtk");
                        }
                }

		//Checks if it is a carriage return to increase the line number
		else if(curChar == '\n')
		{
			charEl = WS;

			counter = 0;

			lineCount++;
		}

		//All else ifs checks if the current character is within the alphabet
		else if(curChar == ' ' || curChar == '\t')
		{
			charEl = WS;

			counter = 0;
		}

		else if(isalpha(curChar))
		{
			if(isupper(curChar))
			{
				charEl = LU;
			}

			else
			{
				charEl = LL;
			}
		}

		else if(isdigit(curChar))
		{
			charEl = D;
		}

		else if(curChar == EOF)
		{
			charEl = END;

			strcat(presToken->tokenInstance, "EOFtk");
		}

		else if(curChar == '_')
		{
			charEl = _;
		}

		else if(curChar == '=')
		{
			charEl = EQ;
		}

		else if(curChar == '<')
                {
                        charEl = LT;
                }

		else if(curChar == '>')
                {
                        charEl = GT;
                }

		else if(curChar == ':')
                {
                        charEl = COL;
                }

		else if(curChar == '+')
                {
                        charEl = PLUS;
                }

		else if(curChar == '-')
                {
                        charEl = MIN;
                }

		else if(curChar == '*')
                {
                        charEl = STAR;
                }

		else if(curChar == '/')
                {
                        charEl = SLSH;
                }

		else if(curChar == '%')
                {
                        charEl = PER;
                }

		else if(curChar == '.')
                {
                        charEl = DOT;
                }

		else if(curChar == '(')
                {
                        charEl = LP;
                }

		else if(curChar == ')')
                {
                        charEl = RP;
                }

		else if(curChar == ',')
                {
                        charEl = COM;
                }

		else if(curChar == '{')
                {
                        charEl = LC;
                }

		else if(curChar == '}')
                {
                        charEl = RC;
                }
		
		else if(curChar == ';')
                {
                        charEl = SEM;
                }

		else if(curChar == '[')
                {
                        charEl = LS;
                }

		else if(curChar == ']')
                {
                        charEl = RS;
                }

		//Characters outside the programs alphabet
		else
                {
                        charEl = EL;
                }

		//If it is not an error or if it hasn't reached a token yet
		if(lexAn[curState][charEl] >= 0 && lexAn[curState][charEl] < 1000)
		{
			curState = lexAn[curState][charEl];

			//If the character is not a white space or in a comment
			if(charEl != WS && charEl != $)
			{
				presToken->tokenInstance[nextChar] = curChar;
			
				nextChar++;
			}


			//Returns to the original state after the comment
			if(charEl == $ && comment == 0)
			{
				curState = prevState;
			}

		
			curChar = lookahead;

			lookahead = getc(input);
		}

		//If there is an error
		else if(lexAn[curState][charEl] < 0)
		{
			presToken->tkID = 1;

			errNum = lexAn[curState][charEl];

			if(errNum != -3)
			{
				printError(errNum, presToken->lineNum, curChar);
			}

			else
			{
				printError(errNum, comStart, curChar);
			}

			repeat = 0;

			curChar = lookahead;
                
                        lookahead = getc(input);

			//if there is an incomplete comment
			if(curState == S36)
			{
				curState = S1;

				comment = 0;
			}
		}

		//If it reached a token
		else
		{
			repeat = 0;

			//If or one of the else ifs the instance equals a key words
			if(strcmp(presToken->tokenInstance, "start") == 0)
                        {
                                presToken->tkID = START_tk;
                        }

			else if(strcmp(presToken->tokenInstance, "stop") == 0)
                        {
                                presToken->tkID = STOP_tk;
                        }

			else if(strcmp(presToken->tokenInstance, "loop") == 0)
                        {
                                presToken->tkID = LOOP_tk;
                        }

			else if(strcmp(presToken->tokenInstance, "void") == 0)
                        {
                                presToken->tkID = VOID_tk;
                        }

			else if(strcmp(presToken->tokenInstance, "var") == 0)
                        {
                                presToken->tkID = VAR_tk;
                        }

			else if(strcmp(presToken->tokenInstance, "return") == 0)
                        {
                                presToken->tkID = RETURN_tk;
                        }

			else if(strcmp(presToken->tokenInstance, "scan") == 0)
                        {
                                presToken->tkID = SCAN_tk;
                        }

			else if(strcmp(presToken->tokenInstance, "out") == 0)
                        {
                                presToken->tkID = OUT_tk;
                        }

			else if(strcmp(presToken->tokenInstance, "program") == 0)
                        {
                                presToken->tkID = PROGRAM_tk;
                        }

			else if(strcmp(presToken->tokenInstance, "if") == 0)
                        {
                                presToken->tkID = IF_tk;
                        }

			else if(strcmp(presToken->tokenInstance, "then") == 0)
                        {
                                presToken->tkID = THEN_tk;
                        }

			else if(strcmp(presToken->tokenInstance, "let") == 0)
                        {
                                presToken->tkID = LET_tk;
                        }

			else
			{
				presToken->tkID = lexAn[curState][charEl];	
			}

//			printf("Name: %s, Num: %d\n", presToken->tokenInstance, presToken->tkID);

			//If the current character is a carriage return or space
			//Skips to the next character to make sure the line number
			//isn't counted twice
                        if(curChar == '\n' || curChar == ' ')
                        {
                                curChar = lookahead;

                                lookahead = getc(input);
                	}
		}
	}

	return presToken;
}

//Prints the error according to the error number
void printError(int errNum, int lineNum, char letter)
{
	//Character in the alphabet but put in incorrectly
	if(errNum == -1)
	{
		printf("Scanner Error: was expecting a lowercase letter, number, or an Operator instead of %c on line %d.\n", letter, lineNum);
	}

	//Character not in the alphabet
	else if(errNum == -2)
	{
		printf("Scanner Error: %c is not included in this language's alphabet on line %d.\n", letter, lineNum);
	}

	//Incomplete comment
	else
	{
		printf("Scanner Error: Second '$' was missing from the comment on line %d.\n", lineNum);
	}
}
