//Created By Philip Keroff

#ifndef PARSER_H
#define PARSER_H

#include "node.h"
#include "token.h"
	
struct node *startParser(FILE *);
struct node *progFunct(FILE *, int);
struct node *blockFunct(FILE *, int);
struct node *varsFunct(FILE *, int);
struct node *exprFunct(FILE *, int);
struct node *AFunct(FILE *, int);
struct node *MFunct(FILE *, int);
struct node *RFunct(FILE *, int);
struct node *statsFunct(FILE *, int);
struct node *mStatFunct(FILE *, int);
struct node *statFunct(FILE *, int);
struct node *inFunct(FILE *, int);
struct node *outFunct(FILE *, int);
struct node *ifFunct(FILE *, int);
struct node *loopFunct(FILE *, int);
struct node *assignFunct(FILE *, int);
struct node *ROFunct(FILE *, int);
void error(int, struct token*);

#endif

/*BNF
<program> -> void <vars> <block>
<block>   -> start <vars> <stats> stop
<vars>    -> empty | var Identifier : Integer <vars>
<expr>    -> <A> / <expr> | <A> * <expr> | <A>
<A>       -> <M> + < A> | <M> - <A> | <M>
<M>       -> - <M> |  <R>
<R>       -> ( <expr> ) | Identifier | Integer
<stats>   -> <stat>  <mStat>
<mStat>   -> empty |  <stat>  <mStat>
<stat>	  -> <in> | <out> | <block> | <if> | <loop> | <assign>
<in>      -> scan  Identifier  .
<out>     -> out [ <expr>  ] .
<if>      -> if ( <expr> <RO> <expr> ) <stat>
<loop>    -> loop ( <expr> <RO> <expr> ) <stat>
<assign>  -> let Identifier  = <expr> .
<RO>      -> < | <  = | >  | >  = | =  =  |   =   

*/
