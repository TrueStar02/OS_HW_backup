#include<sys/types.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
char read_buffer[4096];
char write_buffer[4096];
int main(int argc,char ** argv)
{   int fd[2];
    pid_t pid;
    if(argc!=3)
    {   fprintf(stderr,"Wrong number of arguments");
        return 1;
    }
    FILE * in=fopen(argv[1],"rb");
    FILE * out=fopen(argv[2],"wb");
    if(in==NULL||out==NULL)
    {   fprintf(stderr,"Failed to open the file");
        return 1;
    }
    if(pipe(fd)==-1)
    {   fprintf(stderr,"Pipe failed");
        return 1;
    }
    pid=fork();
    if(pid<0)
    {   fprintf(stderr,"Fork failed");
        return 1;
    }
    else if(pid>0)
    {   close(fd[0]);
        int i=fread(write_buffer,1,4096,in);
        write(fd[1],write_buffer,i);
        close(fd[1]);
    }
    else
    {   close(fd[1]);
        int i=read(fd[0],read_buffer,4096);
        fwrite(read_buffer,1,i,out);
        close(fd[0]);
    }
    return 0;
}
