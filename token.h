//Created By Philip Keroff

#ifndef token_H
#define token_H

enum tokenID{IDENT_tk = 1000, START_tk, STOP_tk, LOOP_tk, VOID_tk, VAR_tk,
             RETURN_tk, SCAN_tk, OUT_tk, PROGRAM_tk, IF_tk, THEN_tk,
             LET_tk, EQ_tk, LESS_tk, GREAT_tk, COL_tk, PLUS_tk, MIN_tk,
             STAR_tk, SLASH_tk, PER_tk, DOT_tk, LPAR_tk, RPAR_tk, 
	     COM_tk, LCUBRA_tk, RCUBRA_tk, SEMCOL_tk, LSQBRA_tk, 
	     RSQBRA_tk, NUM_tk, EOF_tk};

struct token
{
        enum tokenID tkID;
	char *tokenInstance;
	int lineNum;
};

#endif

