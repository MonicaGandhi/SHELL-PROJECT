#include "shell.h"
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
//navo main
char ShellVariable[MAX_VAR][2][1000];
int countVar;
char alias[MAX_VAR][2][100];
int countAlias;
char currentPath[1000];

int findVar(char *s)
{
    int i;
    for(i=0;i<countVar;++i)
        if(strcmp(ShellVariable[i][0],s)==0)
            return i;
    return -1;
}

int findAlias(char *s)
{
    int i;
    for(i=0;i<countAlias;++i)
        if(strcmp(alias[i][0],s)==0)
            return i;
    return -1;
}

void readCmd()
{
    yyparse();
	if(cmds[countCmd-1].numArg==0)
		countCmd--;
}
int buildInCmd()
{
    if(countCmd!=1)
        return 0;

    if(strcmp(tokens[cmds[0].args[0]],"setenv")==0)
    {

        if(cmds[0].numArg!=3)
        {
            int i;
            for(i=3;i<cmds[0].numArg;++i)
            {
                strcat(tokens[cmds[0].args[2]]," ");
                strcat(tokens[cmds[0].args[2]],tokens[cmds[0].args[i]]);
            }
            cmds[0].numArg=3;
        }

        int ind = findVar(tokens[cmds[0].args[1]]);

        if(ind==-1)
        {
            strcpy(ShellVariable[countVar][0],tokens[cmds[0].args[1]]);
            strcpy(ShellVariable[countVar++][1],tokens[cmds[0].args[2]]);
        }
        else
            {
                strcpy(ShellVariable[ind][0],tokens[cmds[0].args[1]]);
                strcpy(ShellVariable[ind][1],tokens[cmds[0].args[2]]);
            }
        return 1;
    }

    if(strcmp(tokens[cmds[0].args[0]],"printenv")==0)
    {
        if(cmds[0].numArg!=1)
        {
            printf("dont in format : printenv");
            return 1;
        }
        int i=0;
        for(i=0;i<countVar;++i)
            printf("%s : %s \n",ShellVariable[i][0],ShellVariable[i][1]);
        return 1;
    }

    if(strcmp(tokens[cmds[0].args[0]],"unsetenv")==0)
    {
        if(cmds[0].numArg!=2)
        {
            return 1;
        }
        int i=0;
        for(i=0;i<countVar;++i)
            if(strcmp(tokens[cmds[0].args[1]],ShellVariable[i][0])==0)
            {
                countVar--;
                strcpy(ShellVariable[i][0],ShellVariable[countVar][0]);
                strcpy(ShellVariable[i][1],ShellVariable[countVar][1]);
            }
        return 1;
    }

    if(strcmp(tokens[cmds[0].args[0]],"cd")==0)
    {
        char buf[200];
        strcpy(buf,currentPath);
        if(cmds[0].numArg==2)
        {

            int i=0;
            if(tokens[cmds[0].args[1]][0]=='/')
                strcpy(currentPath,tokens[cmds[0].args[1]]);
            else
                if(tokens[cmds[0].args[1]][0]=='~')
                {
                    int ind = cmds[0].args[1];

                    if(strlen(tokens[ind])>1&&tokens[ind][1]=='/')
                    {
                        strcpy(currentPath,getenv("HOME"));
                        strcat(currentPath,&tokens[cmds[0].args[1]][1]);
                    }
                    else{
                        char userName[100];
                        int u;
                        for(u=1;u<strlen(tokens[ind]);++u)
                            if(tokens[ind][u]=='/')
                                break;
                            else
                                userName[u-1]=tokens[ind][u];
                        userName[u-1]='\0';

                        struct passwd *pw;
                        uid_t uid;
                        uid = geteuid ();
                        pw = getpwuid (uid);
                        //printf("user name %s  ->  %s\n",userName,pw->pw_name);
                        if(strcmp(userName,pw->pw_name)!=0)
                        {
                            printf("dont exist this file\n");
                            strcpy(currentPath,buf);
                            return 1;
                        }

                        strcpy(currentPath,getenv("HOME"));
                        if(u!=strlen(tokens[ind]))
                            strcat(currentPath,&tokens[ind][u]);
                    }

                }
                else
                {
                strcat(currentPath,"/");
                strcat(currentPath,tokens[cmds[0].args[1]]);
                }
        }
        else
            strcpy(currentPath,getenv("HOME"));
        //printf("current path %s\n",currentPath);
        if(chdir (currentPath)!=0)
        {
                printf("dont exit this file  \n");
                strcpy(currentPath,buf);
                chdir(currentPath);
                return 1;
        }

        return 1;
    }

    if(strcmp(tokens[cmds[0].args[0]],"alias")==0)
    {
        if(cmds[0].numArg>3)
        {
            int u;
            for(u=3;u<cmds[0].numArg;++u)
                {
                strcat(tokens[cmds[0].args[2]]," ");
                strcat(tokens[cmds[0].args[2]],tokens[cmds[0].args[u]]);
                }
            cmds[0].numArg=3;
        }
        if(cmds[0].numArg==1)
        {
            int i;
            for(i=0;i<countAlias;++i)
                printf("%s : %s\n",alias[i][0],alias[i][1]);
            return 1;
        }
        int ind  = findAlias(tokens[cmds[0].args[1]]);
        if(ind!=-1)
        {
            strcpy(alias[ind][1],tokens[cmds[0].args[1]]);
        }
        else
            {
            strcpy(alias[countAlias][0],tokens[cmds[0].args[1]]);
            strcpy(alias[countAlias++][1],tokens[cmds[0].args[2]]);
            }
        return 1;
    }

    if(strcmp(tokens[cmds[0].args[0]],"unalias")==0)
    {
        if(cmds[0].numArg!=2)
        {
            printf("must in alias name\n");
            return 1;	
	}
        int ind  = findAlias(tokens[cmds[0].args[1]]);
	//printf("%d   %s \n",ind,tokens[cmds[0].args[1]]); // now can you compile and use alias and unalias ok i saved it
        if(ind!=-1)
        {
            countAlias--;
            strcpy(alias[ind][0],alias[countAlias][0]);
            strcpy(alias[ind][1],alias[countAlias][1]);
        }
        return 1;
    }

    if(strcmp(tokens[cmds[0].args[0]],"bye")==0)
    {
        exit(-1);
        return 1;
    }

    return 0;
}

int exeNormalCmd()
{
// can you comment out so I can unsetand it well?
    char path[1000];
    int pid;
    int ip,j;
    int pp[100][2];
    for(j=0;j<countCmd;++j)
    {
        pipe(pp[j]);
    }
    for(ip=0;ip<countCmd;++ip)
    {
    char * argv[100];
    int i;
    for(i=0;i<cmds[0].numArg;++i)
        argv[i] = &tokens[cmds[0].args[i]];
    argv[cmds[0].numArg]=NULL;
    int ind = findVar("PATH");
    int flag=0;
    if(ind!=-1)
    {
            int l = strlen(ShellVariable[ind][1]);
            int j=0;
            while(1)
            {
                int t=0;
                while(t+j<l&&ShellVariable[ind][1][t+j]!=':')
                {
                    path[t]=ShellVariable[ind][1][t+j];
                    t++;
                }
                path[t]='\0';
                if(path[strlen(path)-1]!='/')
                    strcat(path,"/");
                strcat(path,tokens[cmds[0].args[0]]);
                //printf("%s\n",path);
                if (access(path,X_OK)==0)
                {
                   flag=1;
                   break;
                }
                if(t+j>=l)
                 break;
                 j+=t+1;
            }
    }
    else{
            strcpy(path,tokens[cmds[0].args[0]]);
               // printf("%s\n",path);
                if (access(path,X_OK)==0)
                {
                   flag=1;
                }
    }
    if(flag==0)
    {
        printf("dont exit this command\n");
        return ;
    }
    if((pid=fork())==0)  // shell create new process by use fork. are you know abut fork. I have heard about it But I dont lknow how fork() works. ok It create new process same shell process. you should read about fork and execp
    {

        if(ip==0&&cmds[0].reInput!=-1)
        {
            /*
                int fin =  open(tokens[cmds[0].reInput], O_RDONLY);

                if(fin<0)
                {
                    printf("cant open file to input\n");
                    return ;
                }
                if(dup2(fin,STDIN_FILENO)<0)
                {
                    printf("cant redirect input file\n");
                    return ;
                }
                */
            if(freopen(tokens[cmds[0].reInput],"r",stdin)==NULL)
            {
                printf("cant reopen input file %d\n",errno);
            }
        }
        if(ip>0)
        {
            close(0);
            //dup(pp[ip][0]);
            dup2(pp[ip][0],STDIN_FILENO);
        }
        if(ip==countCmd-1&&cmds[0].reOutput!=-1)
        {
                /*
                int fout =  open(tokens[cmds[0].reOutput], O_WRONLY|O_CREAT | O_APPEND , 777 );
                if(fout<0)
                {
                    printf("cant open file to output %d\n",fout);
                    return ;
                }

                if(dup2(fout,STDOUT_FILENO)<0)
                {
                    printf("cant redirect output file\n");
                    return ;
                }
                */
                freopen(tokens[cmds[0].reOutput],"w",stdout);
        }
        if(ip<countCmd-1)
        {
            close(1);
            //dup(pp[ip+1][1]);
            dup2(pp[ip+1][1],STDOUT_FILENO);
        }
        /*
        if(chdir (currentPath)!=0)
        {
            printf("cant change current path  %s  %d\n",currentPath,errno);

        }
        */
        if(execvp(path,argv)<0) // what does this check?
        {
           // printf("%d\n",errno);
            printf(": Command not found\n");
            exit(-1);
        }
        printf("completed ");
        exit(0);
    }
    else{
        continue;
    }
    }

    if(!cmds[0].isBackGround)
    {
        int status;
        while (-1 == waitpid(pid, &status, 0));
    }
    for(j=0;j<countCmd;++j)
        close(pp[j]);
}
void checkAlias()
{
    int i,j,n,t,u;
    char path[100];
    int count=0;
    for(i=0;i<countCmd;++i)
    {
        for(j=0;j<cmds[i].numArg;++j)
        {
            int ind = findAlias(tokens[cmds[i].args[j]]);
            if(ind!=-1)
            {
                //printf("meet alias : %s\n",tokens[cmds[i].args[j]]);
                count++;
                for(u=j;u<cmds[i].numArg-1;++u)
                    {
                        cmds[i].args[u]=cmds[i].args[u+1];
                    }
                    cmds[i].numArg--;
                t=0;
                n=0;
                int l=strlen(alias[ind][1]);
                while(1)
                {
                    t=0;
                    while(t+n<l&&alias[ind][1][t+n]!=' ')
                    {
                        path[t]=alias[ind][1][t+n];
                        t++;
                    }
                    path[t]='\0';
                    //printf("%s\n",path);
                    strcpy(tokens[countToken++],path);

                    int u=cmds[i].numArg;
                    while(u>j)
                    {
                        cmds[i].args[u]=cmds[i].args[u-1];
                        u--;
                    }
                    cmds[i].args[j]= countToken-1;
                    j++;
                    cmds[i].numArg++;
                    n+=t+1;
                    if(n>=l)
                    {
                        j--;
                        break;
                    }
                }
            }
        }
    }
    if(count>0)
        checkAlias();
}

void checkVariable()
{
    int i,j,n,t,u;
    char path[100];
    int count=0;
    char s[200];
    char buf[200];
    char var[200];
    for(i=0;i<countCmd;++i)
    {
        for(j=0;j<cmds[i].numArg;++j)
        {
            strcpy(s,tokens[cmds[i].args[j]]);
          //  printf("%s  %d\n",s,j);
            int x,y;
            int flag=0;
            int isVar=0;
            do{
            int l = strlen(s);
            flag=0;
            for(x=0;x+1<l;++x)
            {
                if(s[x]=='$'&&s[x+1]=='{')
                {
                    int jj;
                    for(jj=x+2;jj<l;++jj)
                        if(s[jj]=='}')
                        break;
                  //  printf("%d jj\n",jj);
                    if(jj==l)
                        break;
                    else{
                        //printf("ok replace \n");
                        int u;
                        for(u=x+2;u<jj;++u)
                            var[u-x-2]=s[u];
                        var[jj-x-2]='\0';
                       // printf("var = %s\n",var);
                        int ind = findVar(var);
                        if(ind!=-1)
                        {
                            int m =strlen(ShellVariable[ind][1]);
                            //printf("replace by %s\n",ShellVariable[ind][1]);
                            strcpy(buf,s);
                            for(u=0;u<m;++u)
                                s[x+u]=ShellVariable[ind][1][u];
                            for(u=jj+1;u<l;++u)
                                s[x+m+u-jj-1]=buf[u];
                            s[x+m+l-jj-1]='\0';
                            flag=1;
                        }
                    }
                }
            }
            } while(flag);


            for(u=j;u<cmds[i].numArg-1;++u)
                    {
                        cmds[i].args[u]=cmds[i].args[u+1];
                    }
                    cmds[i].numArg--;
                t=0;
                n=0;
                int l=strlen(s);
               // printf("%s\n",s);
                //printf("j= %d ",j);
                while(1)
                {
                    t=0;
                    while(t+n<l&&s[t+n]!=' ')
                    {
                        path[t]=s[t+n];
                        t++;
                    }
                    path[t]='\0';
                   // printf("%s path\n",path);
                    strcpy(tokens[countToken++],path);

                    int u=cmds[i].numArg;
                    while(u>j)
                    {
                        cmds[i].args[u]=cmds[i].args[u-1];
                        u--;
                    }
                    cmds[i].args[j]= countToken-1;
                 //   printf("%d i\n",j);
                    j++;
                    cmds[i].numArg++;
                    n+=t+1;
                    if(n>=l)
                    {
                        j--;
                        break;
                    }
                }
            }
        }
}


void checkWildcard()
{
    int i,j,n,t,u;
    char path[100];
    int count=0;
    char s[200];
    char buf[200]="";
    char var[200];
    for(i=0;i<countCmd;++i)
    {
        for(j=0;j<cmds[i].numArg;++j)
        {
            strcpy(s,tokens[cmds[i].args[j]]);
            int l = strlen(s);
            int u;
            for(u=0;u<l;++u)
            {
                if(s[u]=='*')
                {
                    int h;
                    for(h=j;h+1<cmds[i].numArg;++h)
                        cmds[i].args[h]=cmds[i].args[h+1];
                    cmds[i].numArg--;

                    DIR *dp;
                    struct dirent *dirp;
                    //printf("current %s\n",currentPath);
                    if( (dp = opendir(currentPath)) == NULL)
                    {
                        perror("Could not open current directory\n");
                        return ;
                    }
                    while( (dirp = readdir(dp)) != NULL)
                    {
                        //printf("file name %s\n",dirp->d_name);
                        int x,y;
                        int flag=1;
                        for(x=0;x<u;++x)
                        if(s[x]!=dirp->d_name[x])
                        {
                            flag=0;
                            break;
                        }
                        int l =strlen(dirp->d_name);
                        for(y=strlen(s)-1;y>x+1;--y)
                            if(s[y]!=dirp->d_name[l-strlen(s)+y])
                            {
                                flag=0;
                            }
                        if(flag==1)
                        {
                            strcpy(tokens[countToken++],dirp->d_name);
                            cmds[i].args[cmds[i].numArg++]= countToken-1;
                        }
                    }
                }

            }
        }
    }
//print();
    for(i=0;i<countCmd;++i)
    {
        for(j=0;j<cmds[i].numArg;++j)
        {
            strcpy(s,tokens[cmds[i].args[j]]);
            int l = strlen(s);
            int u;
            for(u=0;u<l;++u)
            {
                if(s[u]=='?')
                {
                    int h;
                    for(h=j;h+1<cmds[i].numArg;++h)
                        cmds[i].args[h]=cmds[i].args[h+1];
                    cmds[i].numArg--;

                    DIR *dp;
                    struct dirent *dirp;
                    if( (dp = opendir(currentPath)) == NULL)
                    {
                        perror("Could not open current directory\n");
                        return ;
                    }
                    while( (dirp = readdir(dp)) != NULL)
                    {
                        int x,y;
                        int flag=1;
                        int l = strlen(dirp->d_name);
                        int l2 = strlen(s);
                        if(l==l2)
                        {
                            for(x=0;x<l;++x)
                                if(x!=u&&s[x]!=dirp->d_name[x])
                                {
                                    flag=0;
                                    break;
                                }
                            if(flag==1)
                            {
                                strcpy(tokens[countToken++],dirp->d_name);
                                cmds[i].args[cmds[i].numArg++]= countToken-1;
                            }
                        }

                    }
                }

            }
        }
    }
}






void replaceHomePath()
{
    int i,j;
    char s[200];
    for(i=0;i<countCmd;++i)
    {
        for(j=0;j<cmds[i].numArg;++j)
        {
            int ind = cmds[i].args[j];
            strcpy(s,tokens[ind]);
            if(s[0]=='~'&&s[1]=='/')
            {
                strcpy(tokens[ind],getenv("HOME"));
                strcat(tokens[ind],"/");
                strcat(tokens[ind],&s[2]);
            }
        }
    }
}


int main(void) {
char s[1000];
countVar=0;
countAlias=0;
getcwd(currentPath,100);

while(1)
{
	init();
	readCmd();
	checkVariable();
	if(strcmp("unalias",tokens[cmds[0].args[0]])!=0)
		checkAlias();
	// now it is ok
	checkWildcard();

	replaceHomePath();
    //print();
	if(buildInCmd())
        continue;
    exeNormalCmd();

}
return 0;
}
