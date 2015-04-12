#include "shell.h"

int countCmd;
char tokens[MAX_TOKEN][MAX_TOKEN_LENG];
int countToken;
Cmd cmds[MAX_CMD];


/*runs when the inital command is passed.*/
void initCmd(Cmd *cd)
{
	cd->numArg=0;
	cd->isBackGround=NO;
	cd->reInput=cd->reOutput=cd->reError=-1;
}
/*runs the command after the inital pass and counts token and parses to yacc file*/
void init()
{
	countToken=countCmd=0;
	int i;
	for(i=0;i<MAX_CMD;++i)
		initCmd(&cmds[i]);
}
/*prints tokens */ 
void printCmd(Cmd *cd)
{
	printf("args  %d : ",cd->numArg);
	int i=0;
	for(i=0;i<cd->numArg;++i)
		printf("%s  ,  ",tokens[cd->args[i]]);
	printf("\n");
	if(cd->reOutput>=0)
		printf("reoutput : %s\n",tokens[cd->reOutput]);
	if(cd->reInput>=0)
		printf("reinput : %s\n",tokens[cd->reInput]);
	if(cd->reError>=0)
		printf("reerror : %s\n",tokens[cd->reError]);
	if(cd->isBackGround)
		printf("this run in back ground \n");
}
/*printing command counts */

void print()
{
	int i;
	for(i=0;i<countCmd;++i)
	{
		printf("Cmd %d :  \n",i);
		printCmd(&cmds[i]);
	}
}

