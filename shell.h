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
struct cmd
{
	int args[MAX_ARG_NUM];
	int numArg;
	int reInput;
	int reOutput;
	int reError;
	int isBackGround;
};





extern int countCmd;
extern char tokens[MAX_TOKEN][MAX_TOKEN_LENG];
extern int countToken;
extern Cmd cmds[MAX_CMD];
extern char ShellVariable[MAX_VAR][2][1000];
extern int countVar;



void initCmd(Cmd *cd);

void init();
void print();
#endif
