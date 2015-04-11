%{
#include <stdlib.h>
#include "shell.h"
void yyerror(char *);
#include "y.tab.h"
extern char word[1000][100];
extern int numword;
void double_copy(char *a,char *b)
{
	int l = strlen(b);
	if(b[0]!='\"'||b[l-1]!='\"')
		return ;
	int i;
	for(i=0;i<l-2;++i)
		a[i]=b[i+1];
	a[l-2]='\0';
	return ;
}
%}
%%

2>&1 {/* 2 is errror and 1 is terminal */
	strcpy(tokens[countToken++],yytext);
	yylval=countToken-1;	
	return REERROR_OUT; 
	}


2>[^ \<\>\n\|\"\\\&\t\x20]+	{
		strcpy(tokens[countToken++],yytext);
		yylval=countToken-1;
		return REERROR_FILE;/*exclude all metacharacters tab and space */
	}


\"[^\"]*\"	{
		double_copy(tokens[countToken++],yytext);
		yylval=countToken-1;
		return WORD;
	}

[^ \<\>\n\|\"\\\&\t\x20]*	{
		strcpy(tokens[countToken++],yytext);
		yylval=countToken-1;
		return WORD;
		}
\n|\||\<|\>|\& return *yytext;


\\ return SPECIAL;
\x20|\t ;


. 	yyerror("invalid character");
%%
int yywrap(void) {
return 1;
}
