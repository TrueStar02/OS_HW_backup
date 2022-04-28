#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
char cmd[13]="kill -9     ";
int main()
{   pid_t pid;
    pid=fork();
    if(pid<0)
    {   fprintf(stderr,"Failed to folk a process!");
        exit(1);
    }
    else if(pid==0)
    {   printf("I am child process!\n");
        exit(0);
    }
    else
    {   printf("I am father process,I am sleeping\n");
        sleep(10);
        system("ps -o pid,ppid,state,tty,command");
        printf("Father process will be killed\n");
        sprintf(cmd+8,"%d",getpid());
        system(cmd);
        return 0;
    }
}