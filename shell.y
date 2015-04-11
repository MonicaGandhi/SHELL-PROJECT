%{
#include <stdio.h>
#include "shell.h"

int yylex(void);
void yyerror(char *);

%}
%token WORD SPECIAL REERROR_OUT REERROR_FILE
%%
program:
	program cmd_pipe_rein_reout_reerror_back '\n'  {  countCmd++; YYACCEPT;}
	|
	; 

rein : 
	rein '<' WORD	{ $$=$3; }
	|		{ $$ = -1; }
	;
reout:
	reout '>' WORD	{ $$=$3; }
	|	reout	'>' '>' WORD	{ $$=$4; }
	|				{ $$ = -1; }
	;

reerror:
	reerror REERROR_OUT	{ $$=$1; }
	|	reerror	REERROR_FILE	{ $$=$1; }
	|				{ $$ = -1; }
	;
backG: 
	backG	'&'		{ $$=YES;}
	|			{ $$=NO;}
	;

arg: 	WORD	{ $$=$1;}
	;
listarg:
	listarg arg { $$=$1; cmds[$1].args[cmds[$1].numArg++]=$2; }
	| 	arg	{ $$=countCmd; cmds[$$].args[cmds[$$].numArg++]=$1; countCmd++; }
	;	
cmd_pipe:
	cmd_pipe '|'	listarg { }
	|	listarg		{  }
	;

cmd_pipe_rein:
	cmd_pipe rein	{cmds[0].reInput = $2;	};



cmd_pipe_rein_reout:
	cmd_pipe_rein reout	{	cmds[0].reOutput= $2; }
	;

cmd_pipe_rein_reout_reerror:
	cmd_pipe_rein_reout reerror	{	cmds[0].reError=$2; }
	;

cmd_pipe_rein_reout_reerror_back:
	cmd_pipe_rein_reout_reerror backG	{ cmds[0].isBackGround = $2; }
	;
	


%%
void yyerror(char *s) {
fprintf(stderr, "%s\n", s);
}



