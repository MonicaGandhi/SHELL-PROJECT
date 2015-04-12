#ifndef SHELL_H
#define SHELL_H
#include <stdio.h>
#include <stdlib.h>
#define MAX_ARG_NUM 20
#define MAX_ARG_LENG 200
#define MAX_CMD 50
#define MAX_TOKEN 2000
#define MAX_TOKEN_LENG 100
#define NO 0
#define YES 1
#define MAX_VAR 50
#define Cmd struct cmd
/*using struct to grouped list of variables to be placed under one name in a block of memory,
allowing the different variables to be accessed via a single pointer*/
struct cmd
{
	int args[MAX_ARG_NUM];// array of arguments pass
	int numArg; // number of arguments	
	int reInput;// redirectional input
	int reOutput;//redirectional output
	int reError;// redirectional error
	int isBackGround; // checks if background is running 
};
/* using extern to explicity define the following variables */
extern int countCmd; //command count	
extern char tokens[MAX_TOKEN][MAX_TOKEN_LENG]; //checks the token length entered
extern int countToken;// token counter	
extern Cmd cmds[MAX_CMD]; 
extern char ShellVariable[MAX_VAR][2][1000];
extern int countVar;

void initCmd(Cmd *cd);
void init();
void print();
#endif




